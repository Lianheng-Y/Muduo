#include "AsyncLogging.h"
#include "Timestamp.h"
#include <chrono>
#include <stdio.h>

AsyncLogging::~AsyncLogging()
{
    stop();
}

void AsyncLogging::start()
{
    bool expected = false;
    if (running_.compare_exchange_strong(expected, true))
    {
        thread_.start();
    }
}

void AsyncLogging::stop()
{
    bool expected = true;
    if (running_.compare_exchange_strong(expected, false))
    {
        cond_.notify_one();
        thread_.join();
    }
}

AsyncLogging::AsyncLogging(const std::string& basename, off_t rollSize, int flushInterval)
    : flushInterval_(flushInterval)
    , running_(false)
    , basename_(basename)
    , rollSize_(rollSize)
    , thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging")
    , currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char* logline, int len)
{
    if (logline == nullptr || len <= 0)
    {
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if (!running_)
    {
        return;
    }
    if (static_cast<size_t>(currentBuffer_->avail()) >= static_cast<size_t>(len))
    {
        currentBuffer_->append(logline, len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuffer_));

        if (nextBuffer_)
        {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else
        {
            currentBuffer_.reset(new Buffer); // Rarely happens
        }
        currentBuffer_->append(logline, len);
        cond_.notify_one();
    }
}

void AsyncLogging::threadFunc()
{
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    FILE* fp = fopen(basename_.c_str(), "ae");
    if (!fp)
    {
        fprintf(stderr, "AsyncLogging::threadFunc: failed to open file %s\n", basename_.c_str());
        return;
    }

    off_t writtenBytes = 0;
    unsigned int rollIndex = 0;

    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (buffers_.empty() && running_)
            {
                cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
            }
            if (currentBuffer_->length() > 0)
            {
                buffers_.push_back(std::move(currentBuffer_));
                currentBuffer_ = std::move(newBuffer1);
            }
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        if (buffersToWrite.empty() && !running_)
        {
            break;
        }

        if (buffersToWrite.size() > 25)
        {
            // Drop excessive logs
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for (const auto& buffer : buffersToWrite)
        {
            if (rollSize_ > 0 && writtenBytes > 0
                && writtenBytes + buffer->length() > rollSize_)
            {
                fclose(fp);
                const std::string rolledName = basename_ + "." + std::to_string(++rollIndex);
                fp = fopen(rolledName.c_str(), "ae");
                if (!fp)
                {
                    fprintf(stderr, "AsyncLogging::threadFunc: failed to open file %s\n", rolledName.c_str());
                    return;
                }
                writtenBytes = 0;
            }
            fwrite(buffer->data(), 1, buffer->length(), fp);
            writtenBytes += buffer->length();
        }

        if (buffersToWrite.size() > 2)
        {
            // drop non-b1, b2, reuse them as newBuffer1, newBuffer2
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        fflush(fp);
    }
    fflush(fp);
    fclose(fp);
}

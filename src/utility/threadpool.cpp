/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/utility/threadpool.hpp>

#include <new>
#include <thread>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

threadpool::threadpool(size_t number_threads, thread_priority priority)
  : work_(nullptr)
{
    spawn(number_threads, priority);
}

threadpool::~threadpool()
{
    shutdown();
}

void threadpool::spawn(size_t number_threads, thread_priority priority)
{
    for (size_t i = 0; i < number_threads; ++i)
        spawn_once(priority);
}

void threadpool::spawn_once(thread_priority priority)
{
    if (work_ == nullptr)
        work_ = new boost::asio::io_service::work(ios_);

    const auto action = [this, priority]
    {
        set_thread_priority(priority);
        ios_.run();
    };

    threads_.push_back(std::thread(action));
}

void threadpool::stop()
{
    ios_.stop();
}

void threadpool::shutdown()
{
    if (work_ != nullptr)
        delete work_;

    work_ = nullptr;
}

void threadpool::join()
{
    for (auto& thread: threads_)
        if (thread.joinable())
            thread.join();
}

boost::asio::io_service& threadpool::service()
{
    return ios_;
}

const boost::asio::io_service& threadpool::service() const
{
    return ios_;
}

} // namespace libbitcoin


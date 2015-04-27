
// Copyright 2015 The Crisp Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CRISP_CHANNEL_H_
#define CRISP_CHANNEL_H_

#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <iostream>

namespace crisp {

template <typename t>
class Channel {
public:
	Channel(int max) : alive_(true), max_count(max) {}

	bool Get(t *item) {
		// aquire mutex & wait for empty.
		std::unique_lock<std::mutex> lock(mut);
		while (count_ == 0 && alive_) {
			empty.wait(lock);
		}

		if (!alive_ && queue.empty()) {
			return false;
		}

		*item = queue.front();
		queue.pop();
		--count_;

		lock.unlock();
		full.notify_one();
		return true;
	}

	void Put(const t& item) {
		// aquire mutex & wait for empty.
		std::unique_lock<std::mutex> lock(mut);
		while (count_ == max_count && alive_) {
			full.wait(lock);
		}

		if (!alive_) {
			return;
		}

		queue.push(item);

		++count_;
		lock.unlock();
		empty.notify_one();
	}

	void Kill() {
		alive_ = false;
		// wake up all waiting because death.
		full.notify_all();
		empty.notify_all();
	}

	bool alive() const {
		return alive_;
	}
private:
	std::mutex mut;
	std::queue<t> queue;

	// dead queue.
	std::atomic<bool> alive_;

	// atomic access full/empty
	const int max_count;
	std::atomic<int> count_ = {0};

	std::condition_variable full;
	std::condition_variable empty;
};

} // namespace crisp

#endif // CRISP_CHANNEL_H_

#ifndef _BumblebeeContainer_H_
#define _BumblebeeContainer_H_

#include <memory>
#include <vector>
#include <functional>

#include "BumblebeeObject.h"

#define INITIAL_RESERVE 10

// Custom container for most bumblebee related data
template<typename T>
class BumblebeeContainer {
public:
	typedef std::shared_ptr<BumblebeeContainer> ptr;
	typedef std::shared_ptr<T> type_ptr;
	typedef std::unique_ptr<T> type_uptr;
	typedef std::pair<bool, T> pair_type;
	typedef std::function<void(unsigned int)> callback_type;

	// Custom iterator, only exposes flagged data
	class BumblebeeIterator {
	public:
		BumblebeeIterator(unsigned int pos, std::vector<pair_type>* data) {
			this->pos = pos;
			_data = data;
		};
		~BumblebeeIterator() { };

		const bool operator!=(const BumblebeeIterator& rhs)  {
			return pos != rhs.pos;
		}

		const type_ptr operator*()  {
			return std::make_shared<T>(_data->at(pos).second);
		}

		// Advance iterator, skipping non-flagged data
		const BumblebeeIterator& operator++() {
			do {
				pos++;
				if (pos == _data->size()) {
					return *this;
				}
			} while (!_data->at(pos).first);
			return *this;
		}

		type_ptr operator->() {
			return std::make_shared<T>(_data->at(pos).second);
		}

		unsigned int pos;

	private:
		std::vector<pair_type>* _data;
	};

	BumblebeeContainer() : num_objects(0) {
		_data.reserve(INITIAL_RESERVE);
	};
	~BumblebeeContainer() { };

	T* at(unsigned int id) {
		if (id < _data.size()) {
			return &_data.at(id).second;
		}
		return nullptr;
	}

	// Add object to container
	unsigned int add(type_ptr obj) { 
		unsigned int internal_index = 0;
		for (pair_type& member : _data) {
			// Replaces any non-flagged data first
			if (!member.first) {
				member.first = true;
				member.second = *obj;

				if (object_added_callback) {
					object_added_callback(internal_index);
				}
				return internal_index;
			}
			++internal_index;
		}

		// Entire collection is flagged, add a new element
		unsigned int index = num_objects;
		pair_type pair = std::make_pair(true, *obj);
		_data.push_back(pair);
		if (object_added_callback) {
			object_added_callback(index);
		}
		++num_objects;
		return index;
	};

	// Unflags data instead of deleting it
	BumblebeeIterator remove(unsigned int id) {
		if (object_removed_callback) {
			object_removed_callback(id);
		}
		_data[id].first = false;		
		return BumblebeeIterator(id-1, &_data);
	}

	size_t size() {
		return _data.size();
	}

	BumblebeeIterator begin() {
		unsigned int begin_index = 0;
		if (_data.empty()) {
			return BumblebeeIterator(0, &_data);
		}
		while (!_data.at(begin_index).first) {
			++begin_index;
		}
		return BumblebeeIterator(begin_index, &_data);
	}

	BumblebeeIterator end() {
		if (_data.empty()) {
			return BumblebeeIterator(0, &_data);
		}
		return BumblebeeIterator(_data.size(), &_data);
	}	

	const bool empty() {
		return _data.empty();
	}

	callback_type object_added_callback;
	callback_type object_removed_callback;

private:
	std::vector<pair_type> _data;
	unsigned int num_objects;

};

#endif
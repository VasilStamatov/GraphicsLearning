#pragma once

#include <vector>
#include <queue>

template <typename T>
T& access(T& _val) { return _val; }

template <typename T>
T& access(T* _val) { return *_val; }

//Implement a heap (priority queue) using std heap functions 
template <class T, class Container = std::vector<T>, class Compare = std::less<T>>
class Heap
{
public:
		Heap(const Container& _container = Container(), const Compare& _comparator = Compare()) :
				m_container(_container), m_comparator(_comparator)
		{
				//Convert the existing container as a heap
				std::make_heap(m_container.begin(), m_container.end(), m_comparator);
		}
		//Cleanup
		~Heap() { m_container.clear(); }

		/** \brief Check if the container is empty
		 *		\return true if empty */
		bool IsEmpty() { return m_container.empty(); }

		/** \brief Getter for the container size
	 	*		\return number of elements in the container */
		size_t Size() { return m_container.size(); }

		/** \brief Gets the first element of the container
		 *		\return the first element */
	 T& Front() { return m_container.front(); }

	 T& At(const std::size_t& _index) { return m_container.at(_index); }

		/** \brief Checks if the container constains a specific item
	 	*		\return true if the item is found in the container */
		bool Contains(const T& _item) 
		{
				if (std::find(m_container.begin(), m_container.end(), _item) != m_container.end())
				{
						return true;
				}
				return false;
		}
		
		/** \brief Adds an item to the container and heapifies it */
		void Push(const T& _item)
		{
				m_container.push_back(_item);
				std::push_heap(m_container.begin(), m_container.end(), m_comparator);
		}

		/** \brief Removes an item from the container and heapifies it */
		void Pop()
		{
				std::pop_heap(m_container.begin(), m_container.end(), m_comparator);
				m_container.pop_back();
		}

		void Remove(const T& _item)
		{
				m_container.erase(std::remove(m_container.begin(), m_container.end(), _item), m_container.end());
		}
		void Remove(const std::size_t& _position)
		{
				m_container.erase(m_container.begin() + _position);
		}

		void Clear()
		{
				m_container.clear();
		}

		/** \brief heapifies the container (needed if some internal values/priorities have changed) */
		void UpdateHeap()
		{
				std::make_heap(m_container.begin(), m_container.end(), m_comparator);
		}

private:
		Container m_container; ///< the container (usually std::vector)
		Compare m_comparator; ///< the comparator used for sorting(uses operator< on default, but can be changed
};


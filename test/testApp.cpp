import MacroMgr;
import ConfigMgr;
import fs_common;
import my_converter.str;
#include <string>
#include <iostream>
#include <unordered_map>
#include <ntdll.h>
#include <filesystem>
#include <queue>
using namespace std;

// 定义一个unordered_map来存储NTSTATUS代码和对应的字符串描述
std::unordered_map<NTSTATUS, std::string> ntStatusMap{
	{STATUS_SUCCESS, "Operation successfully completed."},
	{STATUS_WAIT_0, "Wait operation completed."},
	{STATUS_ABANDONED_WAIT_0, "Abandoned wait operation completed."},
	{STATUS_USER_APC, "User-mode asynchronous procedure call (APC) completed."},
	{STATUS_TIMEOUT, "The operation timed out."},
	{STATUS_PENDING, "The operation is still pending."},
	{STATUS_PARTIAL_COPY, "Only part of a ReadProcessMemory or WriteProcessMemory request was completed."},
	{STATUS_INVALID_HANDLE, "An invalid handle was specified."},
	{STATUS_NOT_FOUND, "The specified object was not found."},
	{STATUS_NOT_SUPPORTED, "The request is not supported."},
	{STATUS_ACCESS_DENIED, "Access is denied."},
	{STATUS_INVALID_PARAMETER, "One or more parameters are invalid."},
	{STATUS_NO_MEMORY, "Not enough virtual memory or paging file quota is available to complete the specified operation."},
	{STATUS_CONFLICTING_ADDRESSES, "Address range to unmap overlaps with an address range already unmapped."},
	{STATUS_ACCESS_VIOLATION, "The thread tried to access an inaccessible address."},
	{STATUS_IN_PAGE_ERROR, "The pagefile is corrupted or insufficient memory to carry out the requested operation."},
	{STATUS_PAGEFILE_QUOTA, "The process has exceeded its pagefile quota."},
	{STATUS_INVALID_PAGE_PROTECTION, "The page protection cannot be applied to the virtual memory region."},
	{STATUS_ILLEGAL_INSTRUCTION, "An illegal instruction was attempted."},
	{STATUS_ALLOTTED_SPACE_EXCEEDED, "The allocated space is exceeded."},
	{STATUS_INSUFFICIENT_RESOURCES, "Insufficient resources to complete the API."},
	{STATUS_DISK_FULL, "The disk is full."},
	{STATUS_FILE_LOCK_CONFLICT, "A file lock conflict occurred."},
	{STATUS_NOT_IMPLEMENTED, "The function or variable is not implemented."},
	// 更多状态代码...
};

#include <vector>
#include <algorithm>
using namespace std;

template<typename T, typename Comparator = std::less<T>>
class Heap {
	std::vector<T> data_;
	Comparator comp_;

	void adjustDown(int idx) {
		int n = data_.size();
		
		int child = 2 * idx + 1; // 左子节点的索引
		while (child < n) {
			// 检查是否有右子节点并且是否需要与之交换
			if (child + 1 < n && comp_(data_[child], data_[child + 1])) {
				child++;
			}
			// 如果父节点符合比较器的条件，则退出
			if (comp_(data_[child], data_[idx])) {
				break;
			}
			std::swap(data_[idx], data_[child]);
			idx = child;
			child = 2 * idx + 1;
		}
	}

public:
	explicit Heap(const std::vector<T>& init = {}) : data_(init) {
		for (int i = (data_.size() / 2) - 1; i >= 0; --i) {
			adjustDown(i);
		}
	}

	void insert(const T& value) {
		data_.push_back(value);
		int i = data_.size() - 1;
		int parent = (i - 1) / 2;
		while (i > 0 && comp_(data_[parent], data_[i])) {
			std::swap(data_[i], data_[parent]);
			i = parent;
			parent = (i - 1) / 2;
		}
	}

	T pop() {
		T top = data_[0];
		data_[0] = data_.back();
		data_.pop_back();
		adjustDown(0);
		return top;
	}

	bool empty() const {
		return data_.empty();
	}
};

void test()
{
	// 最大堆
	Heap<int, std::less<int>> maxHeap;
	maxHeap.insert(5);
	maxHeap.insert(3);
	maxHeap.insert(8);
	maxHeap.insert(1);
	std::cout << "Max Heap: ";
	while (!maxHeap.empty()) {
		std::cout << maxHeap.pop() << " ";
	}
	std::cout << "\n";
	std::priority_queue<int, vector<int>, less<int>> heap;
	heap.emplace(5); heap.emplace(3); heap.emplace(8); heap.emplace(1);
	cout << "Heap: " << '\n';
	while (!heap.empty())
	{
		cout << heap.top() << ' ';
		heap.pop();

	}
	cout << '\n';
	
}
int main() {
	test();
	getchar();
	MacroMgr macro_mgr;
	macro_mgr.add(pair{ L"USER_HOME",getUserHomePath() });
	wstring app_home_dir;
	try
	{
		app_home_dir = getAppHomePathW();
	}
	catch (...)
	{
		app_home_dir = filesystem::current_path();//brv::strConvert(getAppHomePathW(AppHomeGetSchema::exeDir));
	}
	ConfigMgr config_mgr(&macro_mgr, L"D:\\fs_guard.toml");


	getchar();
}
#pragma once

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
#include <thread>
#include <memory>
#include <queue>
#include <ctime>

#include <cirigl.h>

#include "Random.h"
#include "Macro.h"

/* Sorting Params to send on algorithm selection */
struct SortingParams
{
	std::string sortingAlgorithmName;
	std::vector<uint32_t>& vectorToSort;
};

/* Data Analization utility struct */
struct DataAnalysis
{
	uint32_t swapCount = 0;
	uint32_t accessCount = 0;
	uint32_t objectCopies = 0;

	void Reset()
	{
		swapCount = 0;
		accessCount = 0;
		objectCopies = 0;
	}
};

/* Default Queue element */
struct QueueElement
{
	std::vector<uint32_t> vectorStatus;
	DataAnalysis dataAnalyzerStatus;
};

/* Singleton Class that sorts a chosen vector with several sorting algorithms */
class SortingEngine
{
public:
	/* Get the static instance of the class */
	static std::shared_ptr<SortingEngine> Get()
	{
		if(!s_Instance)	s_Instance = std::shared_ptr<SortingEngine>(new SortingEngine());
		return s_Instance;
	}
	
	/* Get the render queue object */
	const std::queue<QueueElement>& GetRenderQueue() const { return m_RendererQueue; }

	/* Pop from the render queue */
	void Pop() { m_RendererQueue.pop(); }

	/* Execute the chosen algorithm */
	void operator[](const SortingParams& params)
	{
		if (params.sortingAlgorithmName == "Quick Sort")
			QuickSort(params.vectorToSort, 0, params.vectorToSort.size() - 1);
		else if (params.sortingAlgorithmName == "Merge Sort")
			MergeSort(params.vectorToSort, 0, params.vectorToSort.size() - 1);
		else
			m_ProcedureMap[params.sortingAlgorithmName](params.vectorToSort);
	}


private:
	/* Constructor : Setup the procedure map with the sorting procedures */
	SortingEngine() : m_SkipStep(0u)
	{
		m_ProcedureMap["Bubble Sort"] =				BIND(&SortingEngine::BubbleSort);
		m_ProcedureMap["Selection Sort"] =			BIND(&SortingEngine::SelectionSort);
		m_ProcedureMap["Insertion Sort"] =			BIND(&SortingEngine::InsertionSort);
		m_ProcedureMap["Bogo Sort"] =				BIND(&SortingEngine::BogoSort);
		m_ProcedureMap["Heap Sort"] =				BIND(&SortingEngine::HeapSort);
		m_ProcedureMap["Radix Sort"] =				BIND(&SortingEngine::RadixSort);
		m_ProcedureMap["Shell Sort"] =				BIND(&SortingEngine::ShellSort);
		m_ProcedureMap["Cocktail Shaker Sort"] =	BIND(&SortingEngine::CocktailShakerSort);
		m_ProcedureMap["Shuffle"] =					BIND(&SortingEngine::Shuffle);
	}

	/*
		==================== BUBBLE SORT =========================
				Time Complexity (On Average): O(n^2)
		==========================================================
	*/
	void BubbleSort(std::vector<uint32_t>& vector)
	{
	    m_SkipStep = floor(0.195f * vector.size() - 34);
		uint32_t counter = m_SkipStep;

		for (uint32_t j = 0; j < vector.size() - 1; j++)
			for (uint32_t i = 0; i < vector.size() - j - 1; i++)
				if (vector[i] > vector[i + 1])
				{
					s_DataAnalyzer.accessCount += 2;

					std::swap(vector[i], vector[i + 1]);
					s_DataAnalyzer.swapCount++;
					s_DataAnalyzer.accessCount += 6;
					s_DataAnalyzer.objectCopies += 3;

					skipStep;
				}

		m_RendererQueue.push({ vector, s_DataAnalyzer });
	}

	/*
	   ==================== BOGO SORT ===========================
		     Time Complexity: O(n!) (very inefficient!)
			 NOTE: Since its Time Complexity is O(n!) and 
		   a multithread solution has not been yet implemented 
				there is no way to use this procedure. 
	   ==========================================================
	*/
	void BogoSort(std::vector<uint32_t>& vector)
	{
		auto isSorted = [](std::vector<uint32_t>& vector, int n)
		{
			while (--n > 0)
				if (vector[n] < vector[n - 1])
					return false;
			return true;
		};

		while (!isSorted(vector, vector.size()))
			Shuffle(vector);
	}

	/*
		==================== SELECTION SORT =========================
					Time Complexity (On Average): O(n^2)
		=============================================================
	*/
	void SelectionSort(std::vector<uint32_t>& vector)
	{
		uint32_t counter = m_SkipStep;

		for (uint32_t i = 0; i < vector.size() - 1; i++)
		{
			uint32_t min = i;
			for (uint32_t j = i + 1; j < vector.size(); j++)
				if (vector[j] < vector[min])
				{
					s_DataAnalyzer.accessCount += 2;
					min = j;
				}

			std::swap(vector[i], vector[min]);
			s_DataAnalyzer.swapCount++;
			s_DataAnalyzer.accessCount += 6;
			s_DataAnalyzer.objectCopies += 3;

			m_RendererQueue.push({ vector, s_DataAnalyzer });
		}
	}

	/*
		==================== INSERTION SORT =========================
				  Time Complexity (On Average): O(n^2)
		=============================================================
	*/
	void InsertionSort(std::vector<uint32_t>& vector)
	{
		s_DataAnalyzer.Reset();

		for (uint32_t i = 1; i < vector.size(); i++) {
			uint32_t temp = vector[i];
			s_DataAnalyzer.accessCount++;
			s_DataAnalyzer.objectCopies++;
			int j = i - 1;
			while ((j >= 0) && (vector[j] > temp)) {
				vector[j + 1] = vector[j];
				s_DataAnalyzer.accessCount += 3;
				s_DataAnalyzer.swapCount++;
				s_DataAnalyzer.objectCopies++;
				j--;
			}
			vector[j + 1] = temp;
			s_DataAnalyzer.accessCount++;
			s_DataAnalyzer.objectCopies++;

			m_RendererQueue.push({ vector, s_DataAnalyzer });
		}
	}

	/*
		==================== HEAP SORT =========================
			  Time Complexity (On Average): O(n*log(n))
		========================================================
	*/
	void HeapSort(std::vector<uint32_t>& vector)
	{
		std::function<void(std::vector<uint32_t>&, uint32_t, uint32_t)> heapify = [&heapify](std::vector<uint32_t>& vector, uint32_t n, uint32_t i) -> void
		{
			int largest = i;
			int l = 2 * i + 1;
			int r = 2 * i + 2;

			if (l < n && vector[l] > vector[largest])
			{
				largest = l;
				s_DataAnalyzer.accessCount += 2;
			}

			if (r < n && vector[r] > vector[largest])
			{
				largest = r;
				s_DataAnalyzer.accessCount += 2;
			}

			if (largest != i) {
				std::swap(vector[i], vector[largest]);
				s_DataAnalyzer.accessCount += 6;
				s_DataAnalyzer.objectCopies += 3;
				s_DataAnalyzer.swapCount++;

				heapify(vector, n, largest);
			}
		};

		for (int i = vector.size() / 2 - 1; i >= 0; i--)
			heapify(vector, vector.size(), i);

		for (int i = vector.size() - 1; i > 0; i--) {
			std::swap(vector[0], vector[i]);
			s_DataAnalyzer.accessCount += 6;
			s_DataAnalyzer.objectCopies += 3;
			s_DataAnalyzer.swapCount++;

			m_RendererQueue.push({ vector, s_DataAnalyzer });

			heapify(vector, i, 0);
		}
	}

	/*
		==================== QUICK SORT =========================
			   Time Complexity (On Average): O(n*log(n))
		=========================================================
	*/
	void QuickSort(std::vector<uint32_t>& vector, int low, int high)
	{
		if (low < high)
		{
			uint32_t index = QuickSortPartition(vector, low, high);

			QuickSort(vector, low, index - 1);
			QuickSort(vector, index + 1, high);
		}
	}

	uint32_t QuickSortPartition(std::vector<uint32_t>& vector, int low, int high)
	{
		m_SkipStep = floor(0.004f * vector.size() - 0.8f);
		uint32_t counter = m_SkipStep;

		int pivot = vector[high];
		s_DataAnalyzer.accessCount++;
		s_DataAnalyzer.objectCopies++;
		int i = (low - 1);

		for (int j = low; j < high; j++)
		{
			if (vector[j] < pivot)
			{
				i++;
				std::swap(vector[i], vector[j]);
				s_DataAnalyzer.accessCount += 6;
				s_DataAnalyzer.objectCopies += 3;
				s_DataAnalyzer.swapCount++;

				skipStep;
			}

			s_DataAnalyzer.accessCount++;
		}

		std::swap(vector[i + 1], vector[high]);
		s_DataAnalyzer.accessCount += 6;
		s_DataAnalyzer.objectCopies += 3;
		s_DataAnalyzer.swapCount++;

		m_RendererQueue.push({ vector, s_DataAnalyzer });
		return (i + 1);
	}

	/*
		==================== MERGE SORT =========================
			   Time Complexity (On Average): O(n*log(n))
	    =========================================================
	*/
	void MergeSort(std::vector<uint32_t>& vector, int left, int right)
	{
		if (left >= right) {
			return;
		}

		int half = left + (right - left) / 2;

		MergeSort(vector, left, half);
		MergeSort(vector, half + 1, right);
		MergeSortMerge(vector, left, half, right);
	}

	void MergeSortMerge(std::vector<uint32_t>& vector, int left, int half, int right)
	{
		m_SkipStep = floor(0.01f * vector.size() - 2);
		uint32_t counter = m_SkipStep;

		int n1 = half - left + 1;
		int n2 = right - half;

		std::vector<uint32_t> L(n1), R(n2);

		for (int i = 0; i < n1; i++)
		{
			L[i] = vector[left + i];
			s_DataAnalyzer.accessCount += 2;
			s_DataAnalyzer.objectCopies++;
		}
		for (int j = 0; j < n2; j++)
		{
			R[j] = vector[half + 1 + j];
			s_DataAnalyzer.accessCount += 2;
			s_DataAnalyzer.objectCopies++;
		}

		int i = 0;
		int j = 0;
		int k = left;

		while (i < n1 && j < n2) {
			if (L[i] <= R[j]) {
				vector[k] = L[i];
				s_DataAnalyzer.accessCount += 4;
				s_DataAnalyzer.objectCopies++;

				skipStep;
				i++;
			}
			else {
				vector[k] = R[j];
				s_DataAnalyzer.accessCount += 4;
				s_DataAnalyzer.objectCopies++;

				skipStep;

				j++;
			}
			k++;
		}

		while (i < n1) {
			vector[k] = L[i];
			s_DataAnalyzer.accessCount += 2;
			s_DataAnalyzer.objectCopies++;

			skipStep;

			i++;
			k++;
		}

		while (j < n2) {
			vector[k] = R[j];
			s_DataAnalyzer.accessCount += 2;
			s_DataAnalyzer.objectCopies++;

			skipStep;

			j++;
			k++;
		}

		m_RendererQueue.push({ vector, s_DataAnalyzer });
	}

	/*
		==================== RADIX SORT =========================
				Time Complexity (On Average): O(n*k)
		=========================================================
	*/
	void RadixSort(std::vector<uint32_t>& vector)
	{
		auto getMax = [](std::vector<uint32_t>& vector, int n) -> int
		{
			int mx = vector[0];
			s_DataAnalyzer.accessCount++;
			s_DataAnalyzer.objectCopies++;

			for (int i = 1; i < n; i++)
			{
				if (vector[i] > mx)
				{
					mx = vector[i];
					s_DataAnalyzer.accessCount++;
					s_DataAnalyzer.objectCopies++;
				}
				s_DataAnalyzer.accessCount++;
			}

			return mx;
		};

		auto countSort = [this](std::vector<uint32_t>& vector, int n, int exp)
		{
			m_SkipStep = 5;
			uint32_t counter = m_SkipStep;

			std::vector<uint32_t> output(n);
			int i, count[10] = { 0 };

			for (i = 0; i < n; i++)
				count[(vector[i] / exp) % 10]++;

			s_DataAnalyzer.accessCount += n;

			for (i = 1; i < 10; i++)
				count[i] += count[i - 1];

			for (i = n - 1; i >= 0; i--) {
				output[count[(vector[i] / exp) % 10] - 1] = vector[i];
				count[(vector[i] / exp) % 10]--;

				s_DataAnalyzer.accessCount += 3;
				s_DataAnalyzer.objectCopies++;
			}

			for (i = 0; i < n; i++)
			{
				vector[i] = output[i];
				skipStep;
			}

			m_RendererQueue.push({ vector, s_DataAnalyzer });
			s_DataAnalyzer.accessCount += n;
		};

		int m = getMax(vector, vector.size());

		for (int exp = 1; m / exp > 0; exp *= 10)
			countSort(vector, vector.size(), exp);
	}

	/*
		==================== SHELL SORT =========================
			 Time Complexity (On Average): O(n*log(n))
		=========================================================
	*/
	void ShellSort(std::vector<uint32_t>& vector)
	{
		m_SkipStep = floor(0.02f * vector.size() + 1);
		uint32_t counter = m_SkipStep;

		for (int gap = vector.size() / 2; gap > 0; gap /= 2)
		{
			for (int i = gap; i < vector.size(); i += 1)
			{
				int temp = vector[i];
				s_DataAnalyzer.accessCount++;
				s_DataAnalyzer.objectCopies;

				int j;
				for (j = i; j >= gap && vector[j - gap] > temp; j -= gap)
				{
					vector[j] = vector[j - gap];
					s_DataAnalyzer.accessCount += 3;
					s_DataAnalyzer.swapCount++;
					s_DataAnalyzer.objectCopies++;

					skipStep;
				}

				vector[j] = temp;
				s_DataAnalyzer.accessCount++;
				s_DataAnalyzer.objectCopies++;

				skipStep;
			}

			m_RendererQueue.push({ vector, s_DataAnalyzer });
		}
	}

	/*
		==================== COCKTAIL SHAKER SORT =========================
					Time Complexity (On Average): O(n^2)
		===================================================================
	*/
	void CocktailShakerSort(std::vector<uint32_t>& vector)
	{
		m_SkipStep = floor(0.195f * vector.size() - 34);
		uint32_t counter = m_SkipStep;

		bool swapped = true;
		int start = 0;
		int end = vector.size() - 1;

		while (swapped)
		{
			swapped = false;

			for (int i = start; i < end; ++i)
			{
				if (vector[i] > vector[i + 1]) {
					s_DataAnalyzer.accessCount += 2;
					std::swap(vector[i], vector[i + 1]);
					s_DataAnalyzer.accessCount += 6;
					s_DataAnalyzer.objectCopies += 3;
					s_DataAnalyzer.swapCount++;

					skipStep;

					swapped = true;
				}
			}

			if (!swapped)
				break;

			swapped = false;
			--end;

			for (int i = end - 1; i >= start; --i)
			{
				if (vector[i] > vector[i + 1]) {
					s_DataAnalyzer.accessCount += 2;
					std::swap(vector[i], vector[i + 1]);
					s_DataAnalyzer.accessCount += 6;
					s_DataAnalyzer.objectCopies += 3;
					s_DataAnalyzer.swapCount++;

					skipStep;

					swapped = true;
				}
			}

			++start;
		}

		m_RendererQueue.push({ vector, s_DataAnalyzer });
	}

	/*
		==================== FISHER-YATES ALGORITHM =========================
							  Time Complexity: O(n)
		=====================================================================
	*/
	void Shuffle(std::vector<uint32_t>& vector)
	{
		s_DataAnalyzer.Reset();

		m_SkipStep = floor(0.01f * vector.size() - 2);
		uint32_t counter = m_SkipStep;

		for (int i = vector.size() - 1; i >= 1; i--)
		{
			int j = Random::Get()->GetInt(0, i - 1);
			std::swap(vector[i], vector[j]);
			skipStep;
		}

		m_RendererQueue.push({vector, s_DataAnalyzer});
	}

private:
	
	/* Static Instances */
	static std::shared_ptr<SortingEngine> s_Instance;
	static DataAnalysis s_DataAnalyzer;

	/* 
		m_ProcedureMap: Hash map containing bound procedures, indexed by their names 
		m_RendererQueue: FIFO Container of QueueElements (^^ see above ^^) that is used to render **almost** each step of sorting 
	*/
	std::unordered_map<std::string, std::function<void(std::vector<uint32_t>&)>> m_ProcedureMap;
	std::queue<QueueElement> m_RendererQueue;

	/* 
		A variable representing the number of steps to skip for certain sorting algorithms,
		in order to keep display velocities fast enough
	*/
	uint32_t m_SkipStep;
};

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>

#include "LinkedList.h"

// Please change this according to your own AllDocs directory
#define DIRECTORY "/Users/yagizylldrm/Desktop/C++/CENG218 - LabProject2/CENG218 - LabProject2/AllDocs"

// Also, change these according to your files too
#define MAX_FREQUENCY 70000
#define FILE_COUNT 9598

struct FileInfo
{
	std::string filename;
	int frequency = 0;
	LinkedList<int> lineNumbers;
};

struct WordEntry
{
	std::string word;
	int totalFrequency = 0;
	LinkedList<FileInfo> fileList;
};

struct Operation
{
	std::string action;
	std::string details;
	std::string timestamp;
};

LinkedList<WordEntry> wordIndex;
LinkedList<Operation> operationHistory;

void addWordToIndex(const std::string& word, const std::string& document, int lineNum)
{
	Node<WordEntry>* walk_word = wordIndex.getHead();
	
	while (walk_word != nullptr)
	{
		if (walk_word->data.word == word)
		{
			walk_word->data.totalFrequency++;
			
			Node<FileInfo>* walk_file = walk_word->data.fileList.getHead();
			
			while (walk_file != nullptr)
			{
				if (walk_file->data.filename == document)
				{
					walk_file->data.frequency++;
					walk_file->data.lineNumbers.push_back(lineNum);
					return;
				}
				walk_file = walk_file->next;
			}
			
			FileInfo newFile;
			newFile.filename = document;
			newFile.frequency = 1;
			newFile.lineNumbers.push_back(lineNum);
			
			walk_word->data.fileList.push_back(newFile);
			return;
		}
		walk_word = walk_word->next;
	}
	
	WordEntry newWord;
	newWord.word = word;
	newWord.totalFrequency = 1;
	
	FileInfo newFile;
	newFile.filename = document;
	newFile.frequency = 1;
	newFile.lineNumbers.push_back(lineNum);
	
	newWord.fileList.push_back(newFile);
	
	wordIndex.push_back(newWord);
}

void loadDocuments(const std::string& directory)
{
	for (int i = 1; i <= FILE_COUNT; i++)
	{
		std::string filename = directory + "/" + std::to_string(i) + ".txt";
		std::ifstream file(filename);

		if (!file.is_open()) { std::cout << "Couldn't open file: " << filename << '\n'; continue; }

		std::string line;
		int lineNum = 1;

		while (std::getline(file, line))
		{
			std::string currentWord = "";
			for (char ch : line)
			{
				if (isalpha(ch)) { currentWord += tolower(ch); }
				else
				{
					if (!currentWord.empty())
					{
						addWordToIndex(currentWord, std::to_string(i) + ".txt", lineNum);
						currentWord = "";
					}
				}
			}
			
			if (!currentWord.empty())
			{
				addWordToIndex(currentWord, std::to_string(i) + ".txt", lineNum);
			}
			lineNum++;
		}
		file.close();
	}
}

void createIndex(const std::string& directory)
{
	std::cout << "Indexing documents, please wait...\n";
	auto start = std::chrono::high_resolution_clock::now();

	loadDocuments(directory);
	
	if (wordIndex.getHead() == nullptr)
	{
		throw std::runtime_error("Warning: No documents were indexed! Please check your directory path.\n");
	}

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	std::cout << "Index Creation Time: " << elapsed.count() << " seconds.\n";
}

void searchKeyword(const std::string& keyword)
{
	Node<WordEntry>* walk_word = wordIndex.getHead();
	
	std::string cleanKeyword = "";
	for (char c : keyword) { if (isalpha(c)) { cleanKeyword += tolower(c); } }
	
	if (cleanKeyword.empty())
	{
		std::cout << "Invalid keyword! Input must contain at least one valid letter.\n";
		return;
	}
	
	while (walk_word != nullptr)
	{
		if (walk_word->data.word == cleanKeyword)
		{
			std::cout << "Word \"" << cleanKeyword << "\" found in the following files:\n";
			
			Node<FileInfo>* walk_file = walk_word->data.fileList.getHead();
			int docCount = 1;
			while (walk_file != nullptr)
			{
				std::cout << docCount << ". " << walk_file->data.filename << '\n';
				docCount++;
				walk_file = walk_file->next;
			}
			return;
		}
		walk_word = walk_word->next;
	}
	std::cout << "The keyword was not found in any document.\n";
}

void drawProgressBar(int frequency, int maxFrequency)
{
	int barWidth = 20;
	int progress = (frequency * barWidth) / maxFrequency;
	
	std::cout << "[";
	for (int i = 0; i < progress; i++) { std::cout << "█"; }
	for (int i = progress; i < barWidth; i++) { std::cout << " "; }
	std::cout << "] (" << frequency << "/" << maxFrequency << ")\n";
}

void bubbleSortWords(WordEntry** arr, int size)
{
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = 0; j < size - i - 1; j++)
		{
			if (arr[j]->totalFrequency > arr[j + 1]->totalFrequency)
			{
				WordEntry* temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

void displayTopNMostFrequentWords(int N)
{
	if (N <= 0)
	{
		std::cout << "N must be a positive integer.\n";
		return;
	}
	
	int totalWords = 0;
	Node<WordEntry>* walk = wordIndex.getHead();
	
	while (walk != nullptr)
	{
		totalWords++;
		walk = walk->next;
	}
	
	if (N > totalWords)
	{
		std::cout << "N is larger than the total unique words ("
				  << totalWords
				  << "). Showing all available words instead.\n";
		N = totalWords;
	}
	
	if (totalWords == 0) { std::cout << "No words indexed yet.\n"; return; }
	
	WordEntry** wordsArr = new WordEntry*[totalWords];
	walk = wordIndex.getHead();
	for (int i = 0; i < totalWords; i++)
	{
		wordsArr[i] = &(walk->data);
		walk = walk->next;
	}
	
	bubbleSortWords(wordsArr, totalWords);
	
	std::cout << "\nTop " << N << " Most Frequent Words:\n";
	int count = 0;
	for (int i = totalWords - 1; i >= 0 && count < N; i--)
	{
		std::cout << count + 1 << ". Word: \"" << wordsArr[i]->word << "\" -> ";
		drawProgressBar(wordsArr[i]->totalFrequency, MAX_FREQUENCY);
		count++;
	}

	delete[] wordsArr;
}

void displayTopNLeastFrequentWords(int N)
{
	if (N <= 0)
	{
		std::cout << "N must be a positive integer.\n";
		return;
	}
	
	int totalWords = 0;
	Node<WordEntry>* walk = wordIndex.getHead();
	
	while (walk != nullptr)
	{
		totalWords++;
		walk = walk->next;
	}
	
	if (N > totalWords)
	{
		std::cout << "N is larger than the total unique words ("
				  << totalWords
				  << "). Showing all available words instead.\n";
		N = totalWords;
	}

	if (totalWords == 0)
	{
		std::cout << "No words found in index.\n";
		return;
	}

	WordEntry** wordsArr = new WordEntry*[totalWords];
	walk = wordIndex.getHead();
	for (int i = 0; i < totalWords; i++)
	{
		wordsArr[i] = &(walk->data);
		walk = walk->next;
	}

	bubbleSortWords(wordsArr, totalWords);

	std::cout << "\nTop " << N << " Least Frequent Words:\n";
	int count = 0;
	for (int i = 0; i < totalWords && count < N; i++)
	{
		std::cout << count + 1 << ". Word: \"" << wordsArr[i]->word << "\" -> ";
		drawProgressBar(wordsArr[i]->totalFrequency, MAX_FREQUENCY);
		count++;
	}

	delete[] wordsArr;
}

void displayWordFrequencyWithLineNumbers(const std::string& word)
{
	std::string cleanWord = "";
	for (char c : word) { if (isalpha(c)) cleanWord += tolower(c); }
	
	if (cleanWord.empty())
	{
		std::cout << "Invalid keyword! Input must contain at least one valid letter.\n";
		return;
	}

	Node<WordEntry>* walk_word = wordIndex.getHead();
	while (walk_word != nullptr)
	{
		if (walk_word->data.word == cleanWord)
		{
			std::cout << "\nWord: \"" << cleanWord << "\" (Total Frequency: "
					  << walk_word->data.totalFrequency << ")\n";

			Node<FileInfo>* walk_file = walk_word->data.fileList.getHead();
			while (walk_file != nullptr)
			{
				std::cout << "  Document: " << walk_file->data.filename
						  << " (Frequency: " << walk_file->data.frequency << ")\n    Lines: ";

				Node<int>* walk_line = walk_file->data.lineNumbers.getHead();
				while (walk_line != nullptr)
				{
					std::cout << walk_line->data;
					if (walk_line->next != nullptr) std::cout << ", ";
					walk_line = walk_line->next;
				}
				std::cout << "\n";
				walk_file = walk_file->next;
			}
			return;
		}
		walk_word = walk_word->next;
	}
	std::cout << "Word \"" << word << "\" not found in index.\n";
}

void displayWordFrequencyInDocument(const std::string& word, const std::string& document)
{
	std::string cleanWord = "";
	for (char c : word) { if (isalpha(c)) cleanWord += tolower(c); }

	Node<WordEntry>* walk_word = wordIndex.getHead();
	while (walk_word != nullptr)
	{
		if (walk_word->data.word == cleanWord)
		{
			Node<FileInfo>* walk_file = walk_word->data.fileList.getHead();
			while (walk_file != nullptr)
			{
				if (walk_file->data.filename == document)
				{
					std::cout << "Word \"" << cleanWord << "\" appears "
							  << walk_file->data.frequency << " times in " << document << ".\n";
					return;
				}
				walk_file = walk_file->next;
			}
			std::cout << "Word \"" << cleanWord << "\" does not appear in " << document << ".\n";
			return;
		}
		walk_word = walk_word->next;
	}
	std::cout << "Word \"" << word << "\" not found in index.\n";
}

void displayOperationHistory()
{
	std::cout << "Operation History\n";
	Node<Operation>* walk = operationHistory.getHead();
	int count = 1;
	
	while (walk != nullptr)
	{
		std::cout << count << ". Action: "
				  << walk->data.action
				  << " (" << walk->data.details
		          << ") at "
			      << walk->data.timestamp << '\n';
		count++;
		walk = walk->next;
	}
	
	if (count == 1) { std::cout << "No operations recorded yet.\n"; }
}

void logOperation(const std::string& action, const std::string& details)
{
	Operation op;
	op.action = action;
	op.details = details;
	
	std::time_t t = std::time(nullptr);
	char mbstr[1000];
	if (std::strftime(mbstr, sizeof(mbstr), "%H:%M", std::localtime(&t))) { op.timestamp = mbstr; }
	else { op.timestamp = "N/A"; }
	
	operationHistory.push_back(op);
}

void displayMenu()
{
	std::cout << "Main Menu:\n"
			  << "1. Enter a Single Keyword to List The Document(s) (File Names)\n"
			  << "2. Display the Top N Words that Appeared Most Frequently\n"
			  << "3. Display the Top N Words that Appeared Least Frequently\n"
			  << "4. Display Word Frequency With Line Numbers\n"
			  << "5. Display Operation History\n"
			  << "6. Exit\n";
}

int main()
{
	std::cout << "Simple Document Retrieval System\n";
	
	std::string directory = DIRECTORY;
	createIndex(directory);
	
	int choice = 0;
	while (choice != 6)
	{
		displayMenu();
		std::cout << "\nEnter your choice: ";
		
		if (!(std::cin >> choice))
		{
			std::cout << "Invalid input! Please enter a number.\n";
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			continue;
		}
		
		switch (choice)
		{
			case 1:
			{
				std::string keyword;
				std::cout << "Enter keyword: ";
				
				std::cin.ignore(10000, '\n');
				std::getline(std::cin, keyword);
				if (keyword.find(' ') != std::string::npos)
				{
					std::cout << "Error: Keywords cannot contain spaces! Please enter a single word.\n";
					break;
				}
				
				searchKeyword(keyword);
				logOperation("Search Word", keyword);
				break;
			}
			case 2:
			{
				int n;
				std::cout << "Enter N: ";
				
				if (!(std::cin >> n))
				{
					std::cout << "Invalid input! Please enter a valid integer for N.\n";
					std::cin.clear();
					std::cin.ignore(10000, '\n');
					break;
				}
				
				displayTopNMostFrequentWords(n);
				logOperation("Most Frequent", "Top " + std::to_string(n));
				break;
			}
			case 3:
			{
				int n;
				std::cout << "Enter N: ";
				
				if (!(std::cin >> n))
				{
					std::cout << "Invalid input! Please enter a valid integer for N.\n";
					std::cin.clear();
					std::cin.ignore(10000, '\n');
					break;
				}
				
				displayTopNLeastFrequentWords(n);
				logOperation("Least Frequent", "Top " + std::to_string(n));
				break;
			}
			case 4:
			{
				std::string keyword;
				std::cout << "Enter keyword: ";
				
				std::cin.ignore(10000, '\n');
				std::getline(std::cin, keyword);
				if (keyword.find(' ') != std::string::npos)
				{
					std::cout << "Error: Keywords cannot contain spaces! Please enter a single word.\n";
					break;
				}
				
				displayWordFrequencyWithLineNumbers(keyword);
				logOperation("Word Line Analysis", keyword);
				break;
			}
			case 5:
			{
				displayOperationHistory();
				break;
			}
			case 6:
			{
				continue;
			}
			default:
			{
				std::cout << "Wrong input! Try again.\n";
				break;
			}
		}
	}
	
	std::cout << "Thank you for using our program. Have a nice day!\n";
	
	return 0;
}

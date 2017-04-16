#include "stdafx.h"
#include <iostream>
#include <queue>
#include <map>
#include <climits>
#include <iterator>
#include <algorithm>

const int UniqueSymbols = 1 << CHAR_BIT;
const char* SampleString = "Huffman_code_test_string";

typedef std::vector<bool> HuffCode;
typedef std::map<char, HuffCode> HuffCodeMap;

template<typename T, size_t n>
inline size_t arraySize(const T(&arr)[n])
{
	return n;
}

class INode
{
public:
	const int f;

	virtual ~INode() {}

protected:
	INode(int f) : f(f) {}
};

class InternalNode : public INode
{
public:
	INode *const left;
	INode *const right;

	InternalNode(INode* c0, INode* c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
	~InternalNode()
	{
		delete left;
		delete right;
	}
};

class LeafNode : public INode
{
public:
	const char c;

	LeafNode(int f, char c) : INode(f), c(c) {}
};

struct NodeCmp
{
	bool operator()(const INode* lhs, const INode* rhs) const { return lhs->f > rhs->f; }
};

INode* BuildTree(const int(&frequencies)[UniqueSymbols])
{
	std::priority_queue<INode*, std::vector<INode*>, NodeCmp> trees;

	for (int i = 0; i < UniqueSymbols; ++i)
	{
		if (frequencies[i] != 0)
			trees.push(new LeafNode(frequencies[i], (char)i));
	}
	while (trees.size() > 1)
	{
		INode* childR = trees.top();
		trees.pop();

		INode* childL = trees.top();
		trees.pop();

		INode* parent = new InternalNode(childR, childL);
		trees.push(parent);
	}
	return trees.top();
}

void GenerateCodes(const INode* node, const HuffCode& prefix, HuffCodeMap& outCodes)
{
	if (const LeafNode* lf = dynamic_cast<const LeafNode*>(node))
	{
		outCodes[lf->c] = prefix;
	}
	else if (const InternalNode* in = dynamic_cast<const InternalNode*>(node))
	{
		HuffCode leftPrefix = prefix;
		leftPrefix.push_back(false);
		GenerateCodes(in->left, leftPrefix, outCodes);

		HuffCode rightPrefix = prefix;
		rightPrefix.push_back(true);
		GenerateCodes(in->right, rightPrefix, outCodes);
	}
}

int main()
{
	std::cout << "String to code: " << SampleString << std::endl;
	// Build frequency table
	int frequencies[UniqueSymbols] = { 0 };
	const char* ptr = SampleString;
	while (*ptr != '\0')
		++frequencies[*ptr++];

	INode* root = BuildTree(frequencies);

	HuffCodeMap codes;
	GenerateCodes(root, HuffCode(), codes);
	delete root;
	//Code Table
	for (HuffCodeMap::const_iterator it = codes.begin(); it != codes.end(); ++it)
	{
		std::cout << it->first << " ";
		std::copy(it->second.begin(), it->second.end(),
			std::ostream_iterator<bool>(std::cout));
		std::cout << std::endl;
	}
	//Code String
	std::cout << "Code String: ";
	int i = 0;
	while (SampleString[i] != 0){
		for (HuffCodeMap::const_iterator it = codes.begin(); it != codes.end(); ++it)
		{
			if (SampleString[i] == it->first){
				std::copy(it->second.begin(), it->second.end(),
					std::ostream_iterator<bool>(std::cout));
				std::cout << " ";
			}		
		}
		i++;
	};
	return 0;
}
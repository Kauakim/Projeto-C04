#include <iostream>
#include "btree.h"

using namespace std;

void printTranslations(const list<string>& translations)
{
	for (list<string>::const_iterator it = translations.begin(); it != translations.end(); it++) {
		if (it != translations.begin()) {
			cout << ", ";
		}
		cout << *it;
	}
}

static int compareAlphabetic(const string& leftWord, const string& rightWord)
{
	if (leftWord < rightWord) {
		return -1;
	}
    if (leftWord > rightWord) {
		return 1;
	}
    return 0;
}

static int compareBySize(const Word* leftWord, const Word* rightWord)
{
	if (leftWord->word.length() < rightWord->word.length()) {
		return -1;
	}
    if (leftWord->word.length() > rightWord->word.length()) {
		return 1;
	}
    return compareAlphabetic(leftWord->word, rightWord->word);
}

static int treeHeight(TreeNode* current)
{
	if (current == NULL) {
		return 0;
	}

	int leftHeight = treeHeight(current->left);
	int rightHeight = treeHeight(current->right);
	return 1 + ((leftHeight > rightHeight) ? leftHeight : rightHeight);
}

static TreeNode* rotateRight(TreeNode* current)
{
	TreeNode* leftChild = current->left;
	TreeNode* transferSubtree = leftChild->right;

	leftChild->right = current;
	current->left = transferSubtree;

	return leftChild;
}

static TreeNode* rotateLeft(TreeNode* current)
{
	TreeNode* rightChild = current->right;
	TreeNode* transferSubtree = rightChild->left;

	rightChild->left = current;
	current->right = transferSubtree;

	return rightChild;
}

static TreeNode* rebalance(TreeNode* current)
{
	if (current == NULL) {
		return current;
	}

	int balance = treeHeight(current->left) - treeHeight(current->right);

	if (balance > 1) {
		if (treeHeight(current->left->left) < treeHeight(current->left->right)) {
			current->left = rotateLeft(current->left);
		}
		return rotateRight(current);
	}

	if (balance < -1) {
		if (treeHeight(current->right->right) < treeHeight(current->right->left)) {
			current->right = rotateRight(current->right);
		}
		return rotateLeft(current);
	}

	return current;
}

static TreeNode* find_less_save_right(TreeNode*& current)
{
	if (current->left != NULL) {
		return find_less_save_right(current->left);
	} else {
		TreeNode* copy = current;
		current = current->right;
		return copy;
	}
}

void insert(TreeNode*& current, Word* data, int option)
{
	if (current == NULL) {
		current = new TreeNode;
		current->data = data;
		current->left = NULL;
		current->right = NULL;
	} else if (option == 0) {
		if (compareAlphabetic(data->word, current->data->word) < 0) {
			insert(current->left, data, option);
		} else {
			insert(current->right, data, option);
		}
	} else {
		if (compareBySize(data, current->data) < 0) {
			insert(current->left, data, option);
		} else {
			insert(current->right, data, option);
		}
	}

	current = rebalance(current);
}

bool remove(TreeNode*& current, const string& data, int option) {
	bool removed = false;
    
    if (current == NULL) {
		return false;
	} else if (data == current->data->word) {
		TreeNode* temp = current;
		if (current->right == NULL) {
			current = current->left;
			delete(temp);
		} else if (current->left == NULL) {
			current = current->right;
			delete(temp);
		} else {
			temp = find_less_save_right(current->right);
			current->data = temp->data;
		    delete(temp);
		}
		removed = true;
	} else {
		if (option == 0) {
			if (data < current->data->word) {
				removed = remove(current->left, data, option);
			} else {
				removed = remove(current->right, data, option);
			}
		} else {
			Word probe = Word();
			probe.word = data;
			if (compareBySize(&probe, current->data) < 0) {
				removed = remove(current->left, data, option);
			} else {
				removed = remove(current->right, data, option);
			}
		}
	}

	current = rebalance(current);
	return removed;
}

void show_in_order(TreeNode* current)
{
	if (current != NULL) {
		if (current->left != NULL) {
			show_in_order(current->left);
		}
		cout << current->data->word << " - ";
		printTranslations(current->data->translations);
		cout << " - (" << current->data->coordinates.x << ", " << current->data->coordinates.y << ", " << current->data->coordinates.z << ")" << endl;
		if (current->right != NULL) {
			show_in_order(current->right);
		}
	}
}

void destruct(TreeNode*& current)
{
	if (current != NULL) {
		if (current->left != NULL) {
			destruct(current->left);
		}
		if (current->right != NULL) {
			destruct(current->right);
		}
		delete(current);
		current = NULL;
	}
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

//const int MAX_CHAR = 27;

struct SuffixTreeNode {
    std::map<char, struct SuffixTreeNode *> children;
    struct SuffixTreeNode *suffixLink;
    int start;
    int *end;
    int suffixIndex;
};

typedef struct SuffixTreeNode Node;
std::vector<int> result;
char *text;
Node *root = NULL;
Node *lastNewNode = NULL;
Node *activeNode = NULL;
int activeEdge = -1;
int activeLength = 0;
int remainingSuffixCount = 0;
int leafEnd = -1;
int *rootEnd = NULL;
int *splitEnd = NULL;
int size = -1;

Node *newNode(int start, int *end) {
    Node *node = new Node;
    node->suffixLink = root;
    node->start = start;
    node->end = end;
    node->suffixIndex = -1;
    return node;
}

int edgeLength(Node *n) {
    if(n == root)
        return 0;
    return *(n->end) - (n->start) + 1;
}

int walkDown(Node *currNode) {
    if (activeLength >= edgeLength(currNode)) {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}

void extendSuffixTree(int pos) {
    leafEnd = pos;
    remainingSuffixCount++;
    lastNewNode = NULL;

    while(remainingSuffixCount > 0) {

        if (activeLength == 0)
            activeEdge = pos;
        auto it = activeNode->children.find(text[activeEdge]);
        if (it == activeNode->children.end()) {
            Node *node = newNode(pos, &leafEnd);
            activeNode->children.insert(std::make_pair(text[activeEdge], node));
            if (lastNewNode != NULL) {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        } else {
            Node *next = it->second;
            if (walkDown(next)) {
                continue;
            }
            if (text[next->start + activeLength] == text[pos]) {
                if(lastNewNode != NULL && activeNode != root) {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = NULL;
                }
                activeLength++;
                break;
            }
            splitEnd = new int;

            *splitEnd = next->start + activeLength - 1;
            Node *split = newNode(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;
            split->children[text[pos]] = newNode(pos, &leafEnd);
            next->start += activeLength;
            split->children[text[next->start]] = next;
            if (lastNewNode != NULL) {
                lastNewNode->suffixLink = split;
            }
            lastNewNode = split;
        }
        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        } else if (activeNode != root) {
            activeNode = activeNode->suffixLink;
        }
    }
}
void setSuffixIndexByDFS(Node *n, int labelHeight) {
    if (n == NULL)  return;
    int leaf = 1;
    for (auto it = n->children.begin(); it!= n->children.end(); ++it){
        leaf = 0;
        setSuffixIndexByDFS(it->second, labelHeight + edgeLength(it->second));
    }
    if (leaf == 1) {
        n->suffixIndex = size - labelHeight;
    }
}

void freeSuffixTreeByPostOrder(Node *n) {
    if (n == NULL)
        return;
    for (auto it = n->children.begin(); it !=n->children.end(); ++it)
        freeSuffixTreeByPostOrder(it->second);
    if (n->suffixIndex == -1)
        delete n->end;
    delete n;
}

void buildSuffixTree() {
    size = strlen(text);
    rootEnd = new int;
    *rootEnd = - 1;
    root = newNode(-1, rootEnd);

    activeNode = root;
    for (int i=0; i<size; i++)
        extendSuffixTree(i);
    int labelHeight = 0;
    setSuffixIndexByDFS(root, labelHeight);
}

int traverseEdge(char *str, int idx, int start, int end) {
    int k = 0;
    for(k=start; k<=end && str[idx] != '\0'; k++, idx++) {
        if(text[k] != str[idx])
            return -1;
    }
    if(str[idx] == '\0')
        return 1;
    return 0;
}

int doTraversalToCountLeaf(Node *n) {
    if(n == NULL)
        return 0;
    if(n->suffixIndex > -1) {
        result.push_back(n->suffixIndex + 1);
        return 1;
    }
    int count = 0;
    for (auto it = n->children.begin(); it != n->children.end(); ++it){
        count += doTraversalToCountLeaf(it->second);
    }
    return count;
}

int countLeaf(Node *n) {
    if(n == NULL)
        return 0;
    return doTraversalToCountLeaf(n);
}

int doTraversal(Node *n, char* str, int idx) {
    if(n == NULL) {
        return -1;
    }
    int res = -1;
    if(n->start != -1) {
        res = traverseEdge(str, idx, n->start, *(n->end));
        if(res == -1)
            return -1;
        if(res == 1) {
            countLeaf(n);
            return 1;
        }
    }
    idx = idx + edgeLength(n);
    auto find = n->children.find(str[idx]);
    if (find != n->children.end())
        return doTraversal(n->children[str[idx]], str, idx);
    else
        return -1;
}

int main(int argc, char *argv[]) {
    char *to_search_c;
    int i = 0, text_size = 5000, search_size = 100;
    text = (char*)malloc(sizeof(char)*text_size);
    if (text == NULL){
        std::cout << "ERROR: Memory allocation failed (4)" << std::endl;
        return 0;
    }
    while ((text[i] = getchar()) != '\n') {
        ++i;
        if (i == text_size - 2) {
            text_size = text_size*2;
            text = (char*)realloc(text,sizeof(char)*text_size);
            if (text == NULL){
                std::cout << "ERROR: Memory allocation failed (5)" << std::endl;
                return 0;
            }
        }
    }
    text[i] = '{';
    text[i+1] = '\0';
    text = (char*)realloc(text,sizeof(char)*(i+2));
    buildSuffixTree();
    to_search_c = (char*)malloc(sizeof(char)*search_size);
    if (to_search_c == NULL){
        std::cout << "ERROR: Memory allocation failed (6)" << std::endl;
        return 0;
    }
    int j = 1;
    while (1) {
        i = 0;
        while (isalpha(to_search_c[i] = getchar())) {
            ++i;
            if (i == search_size) {
                search_size = search_size*3/2;
                to_search_c = (char*)realloc(to_search_c, sizeof(char)*search_size);
                if (to_search_c == NULL){
                    std::cout << "ERROR: Memory allocation failed (7)" << std::endl;
                    return 0;
                }
            }
        }
        if (to_search_c[i] == '\n') {
            to_search_c[i] = '\0';
            doTraversal(root, to_search_c, 0);
            if (!result.empty()) {
                std::sort(result.begin(), result.end());
                std::cout << j << ": " << result[0];
                for (std::vector<int>::iterator it = ++result.begin(); it != result.end(); ++it) {
                    std::cout << ", " << *it;
                }
                std::cout << std::endl;
                result.clear();
            }
            ++j;
        }
        else break;

    }
    free(to_search_c);
    free(text);
    freeSuffixTreeByPostOrder(root);
    return 0;
}

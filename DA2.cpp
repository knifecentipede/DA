#include <new>
#include <algorithm>
#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>

using namespace std;

// STRING CLASS
class String{
public:
    String();
    String(const char* s);
    String(const String& s);
    ~String();

    size_t Length() const;
    const char* Str() const;

    friend void print(const String& s);
    String& operator=(const String& s);
    bool operator<(const String& s) const;
    bool operator==(const String& s) const;

private:
    char* mStr;
    void mCopy(const String& s);
    void mCopy2(const char* s);
};

String::~String(){
    delete[] mStr;
}

String::String(){
    mStr = new char[1];
    mStr[0] = '\0';
}

String::String(const char* s){
    mStr = new char[1];
    mStr[0]='\0';
    mCopy2(s);
}

String::String(const String& s){
    mStr = new char[1];
    mStr[0]='\0';
    mCopy(s);
}

size_t String::Length() const{
	return strlen(mStr);
}

const char* String::Str() const{
	return mStr;
}

String& String::operator=(const String& s){
	if (this != &s){
		mCopy(s);
	}
	return *this;
}

void print(const String& s){
    cout << s.mStr << endl;
}

bool String::operator<(const String& s) const
{
	return strcmp(mStr, s.mStr) < 0;
}

void String::mCopy(const String& s){
	delete[] mStr;
	size_t n = s.Length();
    mStr = new char[n + 1];
	strcpy(mStr, s.mStr);
//	mStr[n] = '\0';
}

void String::mCopy2(const char* s){
	delete[] mStr;
	size_t n = strlen(s);
    mStr = new char[n + 1];
	strcpy(mStr, s);
//	mStr[n] = '\0';
}


// AVL TREE
typedef unsigned long long ULL;

class AVLtree {
	struct AVLnode;
	struct AVLnode {
		String key;
		ULL value;
		AVLnode* left;
		AVLnode* right;
		int h;
		AVLnode(const String& key, ULL val): key(key), value(val), left(0), right(0), h(1) {};
	};

	AVLnode* root;

	int height(const AVLnode* n) {
	    return n? n->h : 0;
    }
	int balance(const AVLnode* n) {
	    return height(n->right) - height(n->left);
    }
	void reheight(AVLnode* n) {
        n->h = max(height(n->left), height(n->right)) + 1;
    }

	AVLnode* turn_left(AVLnode* n){
		AVLnode* new_root = n->right;
		n->right = new_root->left;
		new_root->left = n;
		reheight(n);
		reheight(new_root);
		return new_root;
	}

	AVLnode* turn_right(AVLnode* n){
		AVLnode* new_root = n->left;
		n->left = new_root->right;
		new_root->right = n;
		reheight(n);
		reheight(new_root);
		return new_root;
	}

	AVLnode* rebalance(AVLnode* n){
		if(!n) return 0;
		reheight(n);
		if(balance(n) == 2)
		{
			if(balance(n->right) < 0)
				n->right = turn_right(n->right);
			return turn_left(n);
		}
		if(balance(n) == -2)
		{
			if(balance(n->left) > 0)
				n->left = turn_left(n->left);
			return turn_right(n);
		}
		return n;
	}

	void show(AVLnode* n){
        if (n) {
            show(n->left);
            cout << n->value << " ";
            print(n->key);
            show(n->right);
        }
	}

	AVLnode* insert(AVLnode* n, const String& key, ULL value){
		if(!n){
			try { n = new AVLnode(key, value); }
			catch(bad_alloc&){
				cerr << "ERROR: Couldn't insert element" << endl;
				exit(1);
			}
            return n;
		}

		if(key < n->key){
			n->left = insert(n->left, key, value);
        }
		else {
			n->right = insert(n->right, key, value);
        }
        return rebalance(n);
	}

	AVLnode* remove_min(AVLnode* n, AVLnode* iter){
		if(iter->left) {
			iter->left = remove_min(n, iter->left);
        }
		else {
			AVLnode* r = iter->right;
			n->key = iter->key;
			n->value = iter->value;
			delete iter;
			iter = r;
		}
		return rebalance(iter);
	}

	AVLnode* remove(AVLnode* n, const String& key) {
		if(!n) return 0;
		if(key < n->key) {
			n->left = remove(n->left, key);
        }
		else if(n->key < key) {
			n->right = remove(n->right, key);
        }

		else {
			AVLnode *l = n->left;
			AVLnode *r = n->right;
			if(!l && !r) {
				delete n;
				return 0;
			}
			if(!r)
			   return l;
			n->right = remove_min(n, r);
		}
		return rebalance(n);
	}

	AVLnode* search(AVLnode* n, const String& key) {
		if(!n)
			return 0;

        AVLnode* iter = n;
        while (iter) {
			if(key < iter->key){
			//    cout<< "left" << endl;
				iter = iter->left;
            }
			else if(iter->key < key) {
			//    cout<< "right" << endl;
				iter = iter->right;
            }
			else {
            //   cout<< "found" << endl;
				return iter;
            }
		}
		return 0;
	}

	void tree_delete(AVLnode* n) {
		if(n->left)
			tree_delete(n->left);
		if(n->right)
			tree_delete(n->right);
		delete n;
	}

	bool saver(FILE* file, AVLnode* node){
	    bool correct = saver_node(file, node);
	    if (correct) {
	        int end = -1;
            fwrite(&end, sizeof(int), 1, file);
            return 1;
	    }
	    else {
	        return 0;
	    }
	}

	bool saver_node(FILE* file, AVLnode* node){
	    if (node) {
            int len = node->key.Length() + 1;
            fwrite(&len, sizeof(int), 1, file);
            fwrite(node->key.Str(), sizeof(char), len, file);
            fwrite(&node->value, sizeof(ULL), 1, file);
            saver_node(file, node->left);
            saver_node(file, node->right);
	    }
	    return 1;
	}

    bool loader(FILE* file, AVLnode** node){
        while(1) {
            char key[300];
            ULL value;
            int len;
            if (fread(&len, sizeof(int), 1, file) != 1){
                return 0;
            }
            if (len == -1) {
                return 1;
            }
            if (fread(&key, sizeof(char), len, file) != len){
                return 0;
            }
            if (fread(&value, sizeof(ULL), 1, file) != 1){
                return 0;
            }

            *node = insert(*node, String(key), value);
        }
    }


public:
	AVLtree(): root(0) {};

	void add(const String key, ULL value) {
	    root = insert(root, key, value);
    //   show(root);
    }

	void del(const String key) {
	    root = remove(root, key);
    }

	ULL* find(const String key) {
	    AVLnode* n = search(root, key);
		if (n) {
			return &(n->value);
        }
		else
			return 0;
	}

	void Destroy(AVLnode* node){
	    if(node) tree_delete(node);
    }

    bool save(const char* filename){
	    FILE* file = fopen(filename, "wb");
	    if (file == NULL){
	        return 0;
	    }

	    bool res = saver(file, root);
	    fclose(file);
	    return res;
    }

	bool load(const char* filename){
	    FILE* file = fopen(filename, "rb");
	    if (file == NULL){
	        return 0;
	    }

        AVLnode* rootNew = NULL;
        AVLnode** rootArg = &rootNew;
        bool res = loader(file, rootArg);
      //  cout << "Final Tree:\n";
      //  show(*rootArg);
	    fclose(file);

	    if (res) {
 	        Destroy(root);
	        root = *rootArg;
	        return 1;
	    } else {
	        Destroy(rootNew);
	        return 0;
	    }

    }

	~AVLtree(){
	    if(root) tree_delete(root);
    }
};

char lowCase(char chr){
	return chr >= 'A' && chr <= 'Z' ? chr - 'A' + 'a' : chr;
}

bool isLetter(char chr){
	return chr >= 'a' && chr <= 'z';
}

void readLine(char* navi, char* str, ULL* num){
    char chr;
    size_t i = 0;
    chr = getchar();

    if (chr == EOF){
        *navi = 'Q';
        delete[] str;
        return;
    }

    else if (chr == '+' || chr == '-'){
        getchar();
        *navi = chr;

        while (1){
            chr = lowCase(getchar());
            if (!isLetter(chr)) {
                break;
            }
        str[i++] = chr;
        }

        str[i] = '\0';
        if (*navi == '+'){
            *num = 0;
            while ((chr = getchar()) != '\n') {
                *num = (*num) * 10 + chr - '0';
            }
        }
    }

    else if (chr == '!'){
		getchar();
		str[0] = chr;
		while ((chr = getchar()) != ' '){
			str[i++] = chr;
		}
		str[i] = '\0';
		i = 0;
		*navi = str[0];

		while ((chr = getchar()) != '\n'){
			str[i++] = chr;
		}
		str[i] = '\0';
    }

    else {
      //  getchar();
        *navi = 'F';
        str[0] = lowCase(chr);
        i++;

        while ((chr = getchar()) != '\n') {
            str[i++] = lowCase(chr);
        }
        str[i] = '\0';
    }
}

// MAIN
int main(){
    AVLtree avl;
    while (true){
        char navi;
        char *buffer = new char[300];
        ULL num;

        readLine(&navi, buffer, &num);

        if (navi == 'Q'){
            break;
        }

        switch (navi){
            case '+':{
                String key(buffer);
                ULL value = num;
                if (avl.find(key) == NULL){
                    avl.add(key, value);
                    cout << "OK" << endl;
                }
                else {
                    cout << "Exist" << endl;
                }
                break;
            }

            case '-':{
                String key(buffer);
                if (! avl.find(key)){
                    cout << "NoSuchWord" << endl;
                }
                else {
                    avl.del(key);
                    cout << "OK" << endl;
                }
                break;
            }

            case 'F':{
                String key(buffer);
                if (! avl.find(key)){
                    cout << "NoSuchWord" << endl;
                }
                else {
                    cout << "OK: " << *avl.find(key) << endl;
                }
                break;
            }


            case 'S':{
                if (avl.save(buffer)){
                    cout << "OK" << endl;
                }
                else {
                    cout << "ERROR: Couldn't create file" << endl;
                }
                break;
            }

            case 'L':{
                if (avl.load(buffer)){
                    cout << "OK" << endl;
                }
                else {
                    cout << "ERROR: Couldn't load file" << endl;
                }
                break;
            }
        }
        delete[] buffer;
    }
}

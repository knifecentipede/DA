#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


int comp(const void *a,const void  *b) {
    return strcmp(*(char**)a, *(char**)b);
}
int search(char ** a, char* s, int t){
    int lb = 0, rb = t-1, x;
    while (lb <= rb) {
        x = (lb+rb)/2;
        if (strcmp(a[x], s) == 0)
            return x;
        else if (strcmp(a[x], s) < 0)
            lb = x + 1;
        else rb = x - 1;
    }
    return -1;
}
void preBmBc(char **pattern, char** bc, int *indexes, int t, int m) {
    int i, j;
    indexes[search(bc, pattern[m-1], t)] = m;
    for (i = 0; i < m - 1; ++i){
        j = search(bc, pattern[i], t);
        if (j >= 0)
            indexes[j] = m - i - 1;
    }
}
void suffixes(char **x, int m, int *suff) {
   int f, g, i;

   suff[m - 1] = m;
   g = m - 1;
   for (i = m - 2; i >= 0; --i) {
      if (i > g && suff[i + m - 1 - f] < i - g)
         suff[i] = suff[i + m - 1 - f];
      else {
         if (i < g)
            g = i;
         f = i;
         while (g >= 0 && strcmp(x[g], x[g + m - 1 - f]) == 0)
            --g;
         suff[i] = f - g;
      }
   }
}
void preBmGs(char **x, int m, int bmGs[], int *suff) {
   int i, j;

   suffixes(x, m, suff);

   for (i = 0; i < m; ++i)
      bmGs[i] = m;
   j = 0;
   for (i = m - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < m - 1 - i; ++j)
            if (bmGs[j] == m)
               bmGs[j] = m - 1 - i;
   for (i = 0; i <= m - 2; ++i)
      bmGs[m - 1 - suff[i]] = m - 1 - i;
}



int main(){
    char **text, **pattern, **pattern_sorted;
    char c;
    int i, j, *bc_indexes, table_size,pattern_allocated = 1000, pattern_length, line_cnt, word_cnt, index, *bmGs, shift, *line_cnt_array, *word_cnt_array, *suff,s,k,*skip;

    pattern = (char**) malloc(sizeof(char*) * pattern_allocated);
    pattern[0] = (char*) malloc(sizeof(char) * 17);
    i = 0;
    j = 0;
    c = getchar();
    while (c != '\n' && c != EOF) {
        while (!isalpha(c) && c != '\n') {
            c = getchar();
        }
        while (isalpha(c)){
            pattern[i][j++] = tolower(c);
            c = getchar();
        }
        while (!isalpha(c) && c != '\n') {
            c = getchar();
        }
        pattern[i][j] = '\0';
        if (c != '\n') {
            ++i;
            if (i == pattern_allocated){
                pattern_allocated *= 2;
                pattern = realloc(pattern, sizeof(char*) * pattern_allocated);
            }
            pattern[i] = (char*)malloc(sizeof(char) * 17);
            j = 0;
        }
    }
    pattern_length = i+1;
    pattern_sorted = (char**)malloc(sizeof(char*)*pattern_length);
    for (i = 0; i < pattern_length; ++i){
        pattern_sorted[i] = pattern[i];
    }

    qsort(pattern_sorted, pattern_length, sizeof(char*), comp);
    for (i = 0, j = 1; i+j < pattern_length; ++i){

        while (i+j < pattern_length && strcmp(pattern_sorted[i],pattern_sorted[i+j]) == 0){
            ++j;
        }
        if ((j > 1) && (i+j < pattern_length)) {
            pattern_sorted[i+1] = pattern_sorted[i+j];
        }
    }

    table_size = pattern_length - j + 1;
    bc_indexes = (int*)malloc(sizeof(int)*table_size);
    preBmBc(pattern, pattern_sorted, bc_indexes, table_size, pattern_length);
    index = 0;
    line_cnt = 1;
    word_cnt = 1;

    text = (char**) malloc(sizeof(char*) * pattern_length);
    line_cnt_array = (int*)malloc(sizeof(int) * pattern_length);
    word_cnt_array = (int*)malloc(sizeof(int) * pattern_length);
    suff = (int*)malloc(sizeof(int) * pattern_length);
    skip = (int*)malloc(sizeof(int) * pattern_length);
    memset(skip, 0, sizeof(int) * pattern_length);
    bmGs = (int*)malloc(sizeof(int) * pattern_length);
    preBmGs(pattern, pattern_length, bmGs, suff);
    for (i = 0; i < pattern_length; ++i) {
        text[i] = (char*)malloc(sizeof(char) * 17);
    }
    c = getchar();
    for (index = 0; index < pattern_length && c != EOF;) {
    //--------------------------------------------------------------------------------------------
        while(!isalpha(c) && c != '\n' && c != EOF)
            c = getchar();

        if  (isalpha(c)){
            j = 0;
            do {
                text[index][j++] = tolower(c);
                c = getchar();
            } while(isalpha(c));
            text[index][j] = '\0';
            //printf("text%d:%s\n",index,text[index]);
            line_cnt_array[index]=line_cnt;
            word_cnt_array[index]=word_cnt;
            ++index;
            ++word_cnt;
        }
        while (c == '\n'){
            word_cnt = 1;
            ++line_cnt;
            c = getchar();
        }
    //--------------------------------------------------------------------------------------------
    }

    shift = pattern_length;
    while (c != EOF || i == shift) {
        i = pattern_length - 1;
        while (i >= 0) {
            k = skip[i];
            s = suff[i];
            if (k > 0)
                if (k > s) {
                    if (i + 1 == s)
                        i = (-1);
                    else
                        i -= s;
                    break;
                }
                else {
                    i -= k;
                    if (k < s)
                        break;
                }
            else {
                  // printf("strcmp: '%s' '%s' i=%d index=%d \n",pattern[i], text[(i + index)%pattern_length], i, index);
                if (strcmp(pattern[i], text[(i + index)%pattern_length]) == 0)
                    --i;
                else
                    break;
            }
        }
        if (i < 0) {
            printf("%d, %d\n", line_cnt_array[index%pattern_length], word_cnt_array[index%pattern_length]);
            skip[pattern_length - 1] = pattern_length;
            shift = bmGs[0];
        }
        else {
            skip[pattern_length - 1] = pattern_length - 1 - i;
            j = search(pattern_sorted, text[(i + index)%pattern_length], table_size);
            if (j>=0)
                shift = bc_indexes[j] - pattern_length + 1 + i;
            else
                shift = 1 + i;
            if (bmGs[i] > shift) {
                shift = bmGs[i];
            }
         }
         for (i = 0; i < shift && c != EOF;) {
    //--------------------------------------------------------------------------------------------
            while(!isalpha(c) && c != '\n' && c != EOF)
            c = getchar();

            if  (isalpha(c)){
                j = 0;
                do {
                    text[index%pattern_length][j++] = tolower(c);
                    c = getchar();
                } while(isalpha(c));

                text[index%pattern_length][j] = '\0';
                line_cnt_array[index%pattern_length]=line_cnt;
                word_cnt_array[index%pattern_length]=word_cnt;
                ++index;
                ++i;
                ++word_cnt;
            }
            while (c == '\n'){
                word_cnt = 1;
                ++line_cnt;
                c = getchar();
            }
//--------------------------------------------------------------------------------------------
        }
        memcpy(skip, skip + shift, (pattern_length - shift)*sizeof(int));
        memset(skip + pattern_length - shift, 0, shift*sizeof(int));
    }
    for (i = 0; i < pattern_length; ++i) {
        free(pattern[i]);
        free(text[i]);
    }
    free(pattern_sorted);
    free(bc_indexes);
    free(line_cnt_array);
    free(word_cnt_array);
    free(text);
    free(suff);
    free(skip);
    free(pattern);
    free(bmGs);
    return 0;
}

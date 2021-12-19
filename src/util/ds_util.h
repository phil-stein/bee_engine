#pragma once
#ifndef DS_UTIL_H
#define DS_UTIL_H

// array ------------------------------------------------
//		 array[][]
//		 array holding arrays of fixed size ARR_LEN
//		 the sub arrays hold the data
//		 the main array holds the sub arrays
//		 to acces an item of index i 
//			-> array[i / ARR_LEN][i % arrlen(array)]
//		 this way the length is variable but making the 
//		 array longer doesnt mean the pointers change
// 
// ------------------------------------------------------

#define ARR_LEN 8


#define arr_init(arr)					arr[0] = malloc(ARR_LEN * sizeof **(arr));
#define arr_extend(arr, len)			arr[(len / ARR_LEN) + 1] = malloc(len + ARR_LEN * sizeof **(arr));

#define arr_check_extend(arr, len)		if (len == 0) { arr_init(arr, len); }			\
										else if (len % ARR_LEN == 0)					\
										{ arr_extend(arr, len); }

#define arr_add_item(arr, len, item)	arr_check_extend(arr, len);						\
										arr[len / ARR_LEN][len % ARR_LEN] = item; len++;

#define arr_get_item(arr, idx)			arr[idx / ARR_LEN][idx % ARR_LEN]

#define arr_pop_item(len)				len--;


// slightly shorter versions of the functions

#ifndef DS_UTIL_LONG_NAMES

#define arr_add(arr, len, item)		arr_add_item(arr, len, item)

#define arr_get(arr, idx)			arr_get_item(arr, idx)

#define arr_pop(len)				arr_pop_item(len);

#endif

#endif

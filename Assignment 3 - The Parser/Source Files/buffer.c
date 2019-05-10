/*********************************************************
File name: buffer.c
Compiler: MS Visual Studio 2015
Author: Ahmed Afzal, ID 040-845-435
Course: CST 8152 - Compilers, Lab Section: 12
Assignment: 1
Date: 9 February 2018
Professor: Sv.Ranev
Purpose: create the buffer for a compiler which stores data and memory
location
Function list: *b_allocate()
b_addc()
b_clear()
b_free()
b_isfull()
b_limit()
b_capacity()
b_mark()
b_mode()
b_incfactor()
b_load()
b_isempty()
b_eob()
b_getc()
b_print()
*b_compact()
b_rflag()
b_retract()
b_reset()
b_getcoffset()
b_rewind()
*b_location()
****************************************************************/

#include "buffer.h"

/***************************************************************
Purpose: allocate memory for a new buffer on the heap
Author: Ahmed Afzal
History/Version: 1.0
Called functions: calloc(), malloc()
Parameters: init_capacity:
type: short
inc_factor:
type: char
o_mode:
type: char
range: -1, 0, 1
Return value: buffer pointer if success, NULL if error
Algorithm: 1. allocate memory for buffer and array
2. set value for operational mode, capacity and inc_factor
******************************************************************/
Buffer *b_allocate(short init_capacity, char inc_factor, char o_mode)
{
	Buffer *buf;
	unsigned char new_inc = (unsigned char)inc_factor;

	if (init_capacity >= 0 && init_capacity <= MAX_SHRT)
	{
		/* checks if memory has been allocated */
		if ((buf = (Buffer*)calloc(1, sizeof(Buffer))) == NULL)
			return NULL;

		/* memory location for character array */
		if ((buf->cb_head = (char*)malloc(sizeof(char) * init_capacity)) == NULL)
			return NULL;

		if (o_mode == 'f' || new_inc == 0)
		{
			buf->mode = 0;
			buf->inc_factor = 0;
		}
		else if (o_mode == 'f' && new_inc != 0)
		{
			buf->mode = 0;
			buf->inc_factor = 0;
		}
		else if (o_mode == 'a' && (new_inc >= 1 && new_inc <= MAX_A))
		{
			buf->mode = 1;
			buf->inc_factor = new_inc;
		}
		else if (o_mode == 'm' && new_inc >= 1 && new_inc <= MAX_M)
		{
			buf->mode = -1;
			buf->inc_factor = new_inc;
		}
		buf->capacity = init_capacity;

		return buf;
	}
	return NULL;
}

/******************************************************
Purpose: add character to a given file
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: isfull(), b_mode(), realloc()
Parameters: pBD:
type: buffer pointer
value: const
symbol:
type: char
value: FILE character
Return value: NULL if error, pBD if success
Algorithm: 1. add symbol to array
2. check if array is full
3. check which operation mode to use
******************************************************/
pBuffer b_addc(pBuffer const pBD, char symbol)
{
	/* define variable for a new capacity */
	short new_capacity;
	/* define a pointer that is a char pointer */
	char *temp;

	/* set r_flag to 0 */
	pBD->r_flag = 0;

	/* check if buffer pointer is empty or does not equal 0 */
	if (b_isfull(pBD) != 1 && b_isfull(pBD) != 0)
		return NULL;

	/* Proceed when full */
	if (b_isfull(pBD) == 1)
	{
		if (pBD->capacity > SHRT_MAX - 1)
			return NULL;

		if (b_mode(pBD) == 0)
			return NULL;
		else if (b_mode(pBD) == 1)
		{
			new_capacity = (unsigned char)pBD->inc_factor + pBD->capacity;

			/* Check if invalid*/
			if (new_capacity < 0)
				return NULL;

			if (new_capacity > 0 && new_capacity <= SHRT_MAX - 1) {}
			else
				new_capacity = SHRT_MAX - 1;
		}
		else
		{
			short avail_space, new_inc;

			if (pBD->capacity >= MAX_SHRT)
				return NULL;

			avail_space = MAX_SHRT - pBD->capacity;
			new_inc = avail_space * (unsigned char)pBD->inc_factor / 100;
			new_capacity = pBD->capacity + new_inc;

			if (pBD->capacity == new_capacity && pBD->capacity < MAX_SHRT)
				new_capacity = MAX_SHRT;
		}
		/* realloc memory for temp */
		temp = (char*)realloc(pBD->cb_head, sizeof(char) * new_capacity);

		if (temp == NULL)
			return NULL;

		/* store temp in cb_head */
		pBD->cb_head = temp;
		/* Set flag to 1 */
		pBD->r_flag = SET_R_FLAG;
		/* Set current capacity to new capacity */
		pBD->capacity = new_capacity;
	}

	/* store symbol in character buffer, increment addc_offset by 1 */
	pBD->cb_head[pBD->addc_offset++] = symbol;

	return pBD;
}

/******************************************************
Purpose: to re-initialize all appriopriate data members, so
buffer will appear empty and when adding characters,
it will be put at the beginning
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: RT_FAIL1 for error, 0 for success
Algorithm: 1. check if buffer pointer is NULL
2. set appriopriate data members value to 0
******************************************************/
int b_clear(Buffer *const pBD)
{
	/* check for possible run-time error */
	if (!pBD)
		return RT_FAIL1;

	/* re-initialize data members of the Buffer structure */
	pBD->addc_offset = 0;
	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	pBD->eob = 0;
	pBD->r_flag = 0;

	return 0;
}

/*****************************************************************
Purpose: de-alloacate memory of character buffer and buffer struct
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: free()
Parameters: pBD:
type: buffer pointer
value: const
Return value: none
Algorithm: 1. use free() method to de-allocate memory for
cb_head
2. set cb_head to NULL
3. free buffer pointer
*****************************************************************/
void b_free(Buffer *const pBD)
{
	free(pBD->cb_head);
	pBD->cb_head = NULL;
	free(pBD);
}

/******************************************************************
Purpose: check if array is full or not
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: RT_FAIL1 if error, SUCCESS if works
Algorithm: 1. check if addc_offset is > capacity,
return RT_FAIL1 if so
2. check if addc_offset is = to capacity,
return SUCCESS if so
3. if above don't work, return ERROR
*****************************************************************/
int b_isfull(Buffer *const pBD)
{
	if (!pBD || pBD->addc_offset > pBD->capacity)
		return RT_FAIL1;
	if (pBD->addc_offset == pBD->capacity)
		return SUCCESS;
	else
		return ERROR;
}

/******************************************************
Purpose: return current limit of character buffer
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: RT_FAIL1 for error, value of addc_offset if success
Algorithm: 1. check if addc_offset is < 0, if so its an error
2. if not, return value of addc_offset
******************************************************/
short b_limit(Buffer *const pBD)
{
	if (!pBD || pBD->addc_offset < 0)
		return RT_FAIL1;

	return pBD->addc_offset;
}

/******************************************************
Purpose: return current capacity of character buffer
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: value of capacity if success, RT_FAIL1 for error
Algorithm: 1. check for possible error
2. return value of capacity
******************************************************/
short b_capacity(Buffer *const pBD)
{
	if (!pBD || pBD->capacity < 0)
		return RT_FAIL1;

	return pBD->capacity;
}

/******************************************************
Purpose: set markc_offset to mark
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
mark:
type: short
range: 0 to addc_offset
Return value: value of markc_offset after being set
to value of mark, RT_FAIL1 for error
Algorithm: 1. check for errors, if mark is > addc_offset
2. set markc_offset to value of mark
******************************************************/
short b_mark(pBuffer const pBD, short mark)
{
	if (!pBD || mark < 0 || mark > pBD->addc_offset)
		return RT_FAIL1;

	return pBD->markc_offset = mark;
}

/******************************************************
Purpose: return value of mode
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: RT_FAIL1 if error, pointer to mode normally
Algorithm: 1. check for error possiblties, if pointer is null
2. return mode
******************************************************/
int b_mode(Buffer *const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	return pBD->mode;
}

/******************************************************
Purpose: return non-negative value of inc_factor
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: return inc_factor, 0x100 if error
Algorithm: 1. check for error, inc_factor cannot be < 0
2. return inc_factor
******************************************************/
size_t b_incfactor(Buffer *const pBD)
{
	if (!pBD || pBD->inc_factor < 0)
		return 0x100;

	return pBD->inc_factor;
}

/******************************************************
Purpose: load an input file into buffer
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: feof(), fgetc(), addc(), b_limit()
Parameters: fi:
type: FILE pointer
value: const
pBD:
type: buffer pointer
value: const
Return value: return RT_FAIL1 if error, return LOAD FAIL if file
cannot be added, return limit of buffer pointer
Algorithm: 1. check for error
2. use fgetc() to read one character at a time
3. continue until end of file is detected
4. return number of characters added
******************************************************/
int b_load(FILE *const fi, Buffer *const pBD)
{
	int temp;

	if (!pBD)
		return RT_FAIL1;

	/* loop until end of file is detected */
	while (!feof(fi))
	{
		temp = fgetc(fi);

		if (feof(fi))
			break;

		if (b_addc(pBD, (char)temp) == NULL)
			return LOAD_FAIL;
	}
	return b_limit(pBD);
}

/******************************************************
Purpose: check if buffer is empty
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: return SUCESS if add_offset is 0, RT_FAIL1 for error
Algorithm: 1. check for error
2. if addc_offset is 0, return SUCCESS
3. if not, return 0
******************************************************/
int b_isempty(Buffer *const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	if (pBD->addc_offset == 0)
		return SUCCESS;
	else
		return 0;
}

/******************************************************
Purpose: return end of buffer value
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: eob value, RT_FAIL1 for error
Algorithm: 1. check for error
2. return value of eob
******************************************************/
int b_eob(Buffer *const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	return pBD->eob;
}

/******************************************************
Purpose: get character in array
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: returns character at getc_offset, RT_FAIL2 for error
Algorithm: 1. check for possible error
2. if getc_offset = addc_offset, set eob to 1
3. return character at getc_offset
******************************************************/
char b_getc(Buffer *const pBD)
{
	if (!pBD)
		return RT_FAIL2;

	if (pBD->getc_offset == pBD->addc_offset)
	{
		pBD->eob = 1;
		return RT_FAIL1;
	}
	else
		pBD->eob = 0;

	/* returns character at getc_offset and increment by 1*/
	return pBD->cb_head[pBD->getc_offset++];
}

/*****************************************************************
Purpose: print characters of buffer to stdout
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: b_isempty(), b_getc(), b_eob()
Parameters: pBD:
type: buffer pointer
value: const
Return value: number of characters printed, RT_FAIL1 for error
Algorithm: 1. check for error
2. check if buffer is empty
3. print characters while checking if its end of buffer
*******************************************************************/
int b_print(Buffer *const pBD)
{
	char cTemp;

	/* check for a run-time error */
	if (!pBD)
		return RT_FAIL1;

	if (b_isempty(pBD) == 1)
	{
		printf("Empty buffer.\n");
		return SUCCESS;
	}

	do
	{
		cTemp = b_getc(pBD);

		if (b_eob(pBD) == 1)
			break;

		printf("%c", cTemp);

	} while (b_eob(pBD) == 0);

	printf("\n");
	return SUCCESS;
}

/******************************************************
Purpose: shrink or expand capcity of buffer
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: realloc()
Parameters: pBD:
type: buffer pointer
value: const
symbol:
type: char
Return value: pointer of buffer, NULL if error
Algorithm: 1. check for error
2. set new capacity
3. reallocate memory for array
4. update necessary members
5. add symbol to end of character buffer
6. return pointer to buffer
******************************************************/
Buffer *b_compact(Buffer *const pBD, char symbol)
{
	char *temp;

	if (!pBD || pBD->addc_offset > pBD->capacity)
		return NULL;

	pBD->capacity = pBD->addc_offset + 1;

	temp = (char*)realloc(pBD->cb_head, pBD->capacity);

	if (temp == NULL)
		return NULL;

	pBD->cb_head = temp;
	pBD->r_flag = SET_R_FLAG;
	pBD->cb_head[pBD->addc_offset++] = symbol;

	return pBD;
}

/******************************************************
Purpose: return r_flag value
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
Return value: value of r_flag, RT_FAIL1 for error
Algorithm: 1. check for run-time error
2. return value of r_flag
******************************************************/
char b_rflag(Buffer *const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	return pBD->r_flag;
}

/******************************************************
Purpose: decrement getc_offset by 1
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: getc_offset value after subtracting by 1,
RT_FAIL1 for error
Algorithm: 1. check for error, getc_offset cannobt be <= 0
2. return getc_offset value
******************************************************/
short b_retract(Buffer *const pBD)
{
	if (!pBD || pBD->getc_offset <= 0)
		return RT_FAIL1;

	return pBD->getc_offset--;
}

/******************************************************
Purpose: set getc_offset to value of markc_offset
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: value of getc_offset, RT_FAIL1 for error
Algorithm: 1. check for error
2. return getc_offset value after setting to
markc_offset value
******************************************************/
short b_reset(Buffer *const pBD)
{
	if (!pBD || pBD->markc_offset < 0)
		return RT_FAIL1;

	return pBD->getc_offset = pBD->markc_offset;
}

/******************************************************
Purpose: to return getc_offset
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: value of getc_offset, RT_FAIL1 for error
Algorithm: 1. check for error, getc_offset cannot be < 0
2. return getc_offset value
******************************************************/
short b_getcoffset(Buffer *const pBD)
{
	if (!pBD || pBD->getc_offset < 0)
		return RT_FAIL1;

	return pBD->getc_offset;
}

/******************************************************
Purpose: to re-read the buffer again
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: buffer pointer
value: const
Return value: 0 for success, RT_FAIL1 if error
Algorithm: 1. check for error
2. set getc_offset and markc_offset to 0
******************************************************/
int b_rewind(Buffer *const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	pBD->getc_offset = 0;
	pBD->markc_offset = 0;

	return 0;
}

/******************************************************
Purpose: returns pointer to a location of character buffer
Author: Ahmed Afzal
History/Versions: 1.0, Feb 8, 2017
Called functions: none
Parameters: pBD:
type: pointer of Buffer
value: const
loc_offset:
type: short
range: >= 0 and < capacity
Return value: character buffer pointer plus loc_offset,
NULL if error
Algorithm: 1. check for error
2. return the pointer to character buffer
******************************************************/
char *b_location(Buffer *const pBD, short loc_offset)
{
	/* check for run-time error */
	if (!pBD || loc_offset < 0 || loc_offset > pBD->addc_offset)
		return NULL;
	return pBD->cb_head + loc_offset;
}
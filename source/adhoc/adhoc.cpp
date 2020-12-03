
#include "../ubut/ubench.h"
#include "random_words_generator.h"


UBENCH(adhoc,random_words) 
{
	dbj_adhoc::test_random_words_generator();
}

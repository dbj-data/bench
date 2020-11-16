
#include "../ubut/ubench.h"
#include "adhoc/random_words_generator.h"


UBENCH(adhoc,random_words) 
{
	dbj_adhoc::test_random_words_generator();
}

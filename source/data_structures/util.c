/*bit_print(u32 x)
{
	u8 n = 0;
	u32 mask = 0x1;
	while(n < 32)
	{
		printf("%d",x & mask ? 1 : 0);
		mask <<= 1;
		if(!((n+1)%4))
			printf(" ");
		n++;
	}
	printf("\n");
}*/

u8 get_first_set_alt(u32 v)
{
	if(v == 0){ return -1; }
	u8 MultiplyDeBruijnBitPosition[32] = 
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v = (v >> 1) + 1;
	
	return MultiplyDeBruijnBitPosition[(v * 0x077CB531UL) >> 27];
}

u8 get_first_set(u32 x)
{
	if(x == 0){ return -1; }
	x = (x & -x);

	u8 j = 0;

	while(x != 1)
	{
		x = x >> 1;
		j++;
	}
	return j;
}

/*
void memcopy(void *start_src, void *start_targ, u32 size)
{
	u32 i;
	if(start_src < start_targ)
	{
		for(i = 0; i < size; i++)
		{
			*(start_targ + i) = *(start_src + i);
		}
	}
	else if(start_src > start_targ)
	{
		for(i = 0; i < size; i++)
		{
			*(start_targ + (size - i)) = *(start_src + (size - i));
		}
	}
}
*/

void setm(u32 *start, u32 size, u32 val){
  u32 i;
  for(i = 0; i < size; i += 1){
    *(start + i) = val;
  }
}

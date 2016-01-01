typedef struct
{
  u32 max;
  u32 *base;
  u32 *top;
} stack_t;

u32 stack_push(stack_t *s, u32 x){
  if((s->top - s->base >= s->max) && (s->top != s->base - 1)){
    return -1;
  }
  s->top += 1;
  //prints("as");
  //stack_debug_print(s);
  *(s->top) = x;
  //prints("|");print_hex_32(s->top);prints(",");print_hex_32(x);prints("|");
  return 0;
}
u32 stack_pop(stack_t *s){
  if(s->top < s->base){
    return -1;
  }
  s->top -= 1;
  u32 x = *(s->top);
  return x;
}
u32 stack_peek(stack_t *s){
  if(s->top < s->base) return -1;
  return *((s->top));
}
stack_t *stack_create(u32 m){
  stack_t *s = (stack_t *)kmalloc(sizeof(stack_t));
  s->max = m;
  s->base = (u32 *)kmalloc(4*m);
  s->top = s->base - 1;
  //stack_debug_print(s);
  //stack_debug_print(s);
  return s;
}


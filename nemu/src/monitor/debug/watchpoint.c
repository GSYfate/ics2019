#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char* expression){
  WP* ret;
  if(free_!=NULL){
    ret = free_;
    free_ = free_->next;
    ret->next = head;
    head = ret;
    ret->expression=(char*)malloc(sizeof(char)*strlen(expression));
    strcpy(ret->expression,expression);
    bool success;
    ret->value=expr(ret->expression,&success);
    ret->old_value=ret->value;
    ret->enable=true;
  }else{
    printf("no free WP\n");
    assert(0);
  }
}

void free_wp(WP* wp){
  WP* ptr;
  for(ptr=head;ptr!=NULL&&ptr->next!=wp;ptr=ptr->next);
  if(ptr==NULL){
    printf("not find such watchpoint\n");
  }else{
    ptr->next=wp->next;
    wp->next=free_;
    free_ = wp;
    wp->enable=false;
  }
  while(ptr->next != wp)ptr=ptr->next;
}

bool delete_wp(int no){
  WP* ptr;
  bool found_no=false;
  for(ptr=head;ptr!=NULL;ptr=ptr->next){
    if(ptr->NO==no){
      free_wp(ptr);
      found_no=true;
      break;
    }   
  }
  if(!found_no){
    printf("No such an activatd watchpoint\n");
  }
  return found_no;
}

bool change_wp(){
  bool  has_changed=false;
  WP* ptr;
  for(ptr=head;ptr!=NULL;ptr=ptr->next){
    bool success;
    int val = expr(ptr->expression,&success);
    ptr->old_value=ptr->value;
    ptr->value=val;
    if(ptr->value!=ptr->old_value){
      has_changed=true;
    }
  }
  return has_changed;
}

void print_wp(){
  WP* ptr;
  printf("%-32s%-16s%-16s%-8s%-8s\n","expression","old_value","value","change","enable");
  for(ptr=head;ptr!=NULL;ptr=ptr->next){
    char changed=(ptr->old_value==ptr->value)?'N':'Y';
    char enabled=(ptr->enable)?'Y':'N';
    printf("%-32s%-16d%-16d%-8c%-8c\n",ptr->expression,ptr->old_value,ptr->value,changed,enabled);
  }
}
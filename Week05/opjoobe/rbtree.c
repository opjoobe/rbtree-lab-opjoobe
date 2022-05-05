#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void){
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  if (NULL != p){
    /* DEFINE NIL NODE for sentinel */
    p->nil = calloc(1, sizeof(node_t));
    p->nil->color = RBTREE_BLACK;
    /* INITIALIZE RBTREE */
    p->root = p->nil;
  }
  return p;
}

/* free each node for delete_rbtree */
void rbtree_delete_node(rbtree *t, node_t *p){
  if (t->nil != p)
  {
    rbtree_delete_node(t, p->left);
    rbtree_delete_node(t, p->right);
    free(p);
    p = NULL;
  }
}

void delete_rbtree(rbtree *t){
  // TODO: reclaim the tree nodes's memory
  rbtree_delete_node(t, t->root);
  free(t->nil);
  free(t);
  t = NULL;
}

/* LeftRotation for Insertion */
void rbtree_rotate_left(rbtree *t, node_t *p){
  // p = subtree's current root
  node_t *new_p = p->right; 
  // new_p (p->right) will be subtree's new root
  p->right = new_p->left;
  if (t->nil != new_p->left)
    new_p->left->parent = p; // new_p->left will be current root's subtree
  new_p->parent = p->parent; //new_p starts to replace p (success parent)
  
  if (t->nil == p->parent) // if p was root
    t->root = new_p; // new_p will be root, too
  else
  {
    if (p == p->parent->left) // if p was leftChild
      p->parent->left = new_p; // new_p will be leftChild, too
    else
      p->parent->right = new_p; // rightChild --> same as leftChild logic
  }
  p->parent = new_p; // // p becomes new_p's leftchild 
  new_p->left = p; // p becomes new_p's leftchild
}

/* RightRotation for Insertion */
void rbtree_rotate_right(rbtree *t, node_t *p){ 
  // p = subtree's current root
  node_t *new_p = p->left;
  // new_p(p->left) will be subtree's new root
  /* same logic from rbtree_rotate_left */
  p->left = new_p->right;
  if (t->nil != new_p->right)
    new_p->right->parent = p;
  new_p->parent = p->parent;
  if (t->nil != p->parent)
  {
    if (p == p->parent->left)
      p->parent->left = new_p;
    else
      p->parent->right = new_p;
  }
  else
    t->root = new_p;
  p->parent = new_p;
  new_p->right = p;
}

/* Insertion_Fixup */
void rbtree_insert_fixup(rbtree *t, node_t *p){
  node_t *target_node = p, *parent_node, *uncle_node;
  while (target_node->parent->color == RBTREE_RED)
  {
    parent_node = target_node->parent;
    if (parent_node == parent_node->parent->left) // parent is leftChild
    {
      uncle_node = parent_node->parent->right;
      if (uncle_node->color == RBTREE_RED)
      {
        // parent & uncle = RED - RED
        parent_node->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        // change parent & uncle (RED -> BLACK)
        parent_node->parent->color = RBTREE_RED;
        // change grandparent (BLACK -> RED)
        target_node = target_node->parent->parent;
        // now, grandparent_node will be new target
        // go back to WHILE. new fixup needed
      }
      else 
      {
        // parent & uncle = RED - BLACK 
        if (target_node == parent_node->right) // target is rightChild
        { 
          // in this case, it's curved path to grandparent (not straight)
          target_node = parent_node;
          rbtree_rotate_left(t, target_node); 
          // after this IF, it becomes straight path !
        }
        /* use target_node->parent instead of parent_node, because it could have changed in IF */
        // straight path to grandparent : RED-RED-BLACK
        target_node->parent->color = RBTREE_BLACK;
        target_node->parent->parent->color = RBTREE_RED;
        rbtree_rotate_right(t, target_node->parent->parent);
        // fixup completed (target_node's parent isn't RED anymore)
      }
    }
    else
    // same logic with IF above
    {
      uncle_node = parent_node->parent->left;
      if (uncle_node->color == RBTREE_RED)
      {
        parent_node->color = RBTREE_BLACK;
        uncle_node->color = RBTREE_BLACK;
        parent_node->parent->color = RBTREE_RED;
        target_node = target_node->parent->parent;
      }
      else
      {
        if (target_node == parent_node->left)
        {
          target_node = parent_node;
          rbtree_rotate_right(t, target_node); // RIGHTROTATE(t,target_node);
        }
        /* use target_node->parent instead of parent_node, because it could have changed in IF */
        target_node->parent->color = RBTREE_BLACK;
        target_node->parent->parent->color = RBTREE_RED;
        rbtree_rotate_left(t, target_node->parent->parent); // RIGHTROTATE(T, target_node->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key){
  // TODO: implement insert
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t)); // memory allocation for new node
  node_t *now_node = t->root;                                  // target NIL for new node
  node_t *parent_node = t->nil;                                // parent who's pointing the target NIL
  while (t->nil != now_node)
  {
    parent_node = now_node; // necessary because it's impossible to find parent by now->parent (now == t->nil when WHILE breaks)
    if (now_node->key <= key) /* BUG FOUND HERE */  
      now_node = now_node->right; /* same key to left, why not possible.....? */
    else
      now_node = now_node->left; // if same key already exists, put it on Left-Subtree anyway (for duplication TestCase)
  }
  new_node->parent = parent_node;
  if (t->nil == parent_node) // root is the only node with NIL parent
    t->root = new_node; // therefore, new_node should be root
  else if (key < parent_node->key)
    parent_node->left = new_node; // new_node replaces parent's left NIL
  else
    parent_node->right = new_node; // new_node replaces replace parent's right NIL
  /* SETUP for new_node insertion */
  new_node->color = RBTREE_RED; // inserting RED in order to maintain RULE #5 (regarding Black Height)
  new_node->key = key;
  new_node->left = t->nil; // inserting BLACK NIL in order to maintain RULE #4 (No RED-RED)
  new_node->right = t->nil; // same here
  rbtree_insert_fixup(t, new_node); //fixup if new_node(RED)'s parent is RED
  // return t->root;
  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key){
  // TODO: implement find
  node_t *current_node = t->root;
  // same logic with general BST's find()
  while (current_node != t->nil)
  {
    if (key == current_node->key)
      return current_node;
    else if (key < current_node->key)
      current_node = current_node->left;
    else
      current_node = current_node->right;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t){
  // TODO: implement find
  // same logic with general BST's min()
  node_t *min_node = t->root;
  if (t->nil == min_node)
    return NULL;
  while (t->nil != min_node->left)
    min_node = min_node->left;
  return min_node;
}

node_t *rbtree_max(const rbtree *t){
  // TODO: implement find
  // same logic with general BST's max()
  node_t *max_node = t->root;
  if (t->nil == max_node)
    return NULL;
  while (t->nil != max_node->right)
    max_node = max_node->right;
  return max_node;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  /* transplant subtree first */
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  /* also, transplant parent too */
  v->parent = u->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *p){
  node_t *fixup_node = p, *sibling_node;
  while (fixup_node != t->root && fixup_node->color == RBTREE_BLACK)
  {
    // fixup_node = doubly_black_node, sibling_node = fixup's sibling_node
    if (fixup_node == fixup_node->parent->left)
    { // fixup_node = leftChild, sibling_node = rightChild
      sibling_node = fixup_node->parent->right;
      
      /* case 1 : sibling_node = RED */
      if (sibling_node->color == RBTREE_RED)
      {
        sibling_node->color = RBTREE_BLACK;
        sibling_node->parent->color = RBTREE_RED;
        // exchange color between sibling & parent
        rbtree_rotate_left(t, sibling_node->parent);
        sibling_node = fixup_node->parent->right;
        // update sibling after rotation
        // now, case 1 changed to one of case 2~4
      }
      
      /* case 2 ~ 4 : sibling_node = BLACK */ 
      if (sibling_node->left->color == RBTREE_BLACK && sibling_node->right->color == RBTREE_BLACK)
      {
        /* case 2 : sibling node's both child =  BLACK*/   
        sibling_node->color = RBTREE_RED; 
        fixup_node = fixup_node->parent; // fixup's parent becomes new fixup_node
      }
      
      /* case 3, 4 : one of sibling node's child = RED */
      else{
        /* case 3 : sibling_node's leftchild: RED & rightchild: BLACK */
        if(sibling_node->right->color == RBTREE_BLACK){
          sibling_node->left->color = RBTREE_BLACK; // recolor sibling node's leftchild to BLACK
          sibling_node->color = RBTREE_RED; // recolor sibling node to RED
          rbtree_rotate_right(t, sibling_node);
          sibling_node = fixup_node->parent->right; // update sibling after rotation
          // now, case 3 changed to case 4 
        }
        /* case 4 : sibling_node's rightchild : RED */
        // straight formation (RED|BLACK - BLACK - RED)
        sibling_node->color = fixup_node->parent->color; // both RED or BLACK available
        sibling_node->right->color = RBTREE_BLACK; // recolor
        sibling_node->parent->color = RBTREE_BLACK; // recolor 
        // new formation (BLACK - RED|BLACK - BLACK)
        rbtree_rotate_left(t,sibling_node->parent);
        fixup_node = t->root;
      }
    }
    
    /* else = same logic with IF above */
    else{ // fixup_node = rightChild, sibling_node = leftChild // change left<->right
      sibling_node = fixup_node->parent->left;
      /* case 1 */
      if (sibling_node->color == RBTREE_RED){
        sibling_node->color = RBTREE_BLACK;
        sibling_node->parent->color = RBTREE_RED;
        rbtree_rotate_right(t, sibling_node->parent);
        sibling_node = fixup_node->parent->left;
      }
      /* case 2 */ 
      if (sibling_node->right->color == RBTREE_BLACK && sibling_node->left->color == RBTREE_BLACK){
        sibling_node->color = RBTREE_RED;
        fixup_node = fixup_node->parent;
      }
      /* case 3, 4 */
      else{
        /* case 3 */
        if(sibling_node->left->color == RBTREE_BLACK){
          sibling_node->right->color = RBTREE_BLACK;
          sibling_node->color = RBTREE_RED;
          rbtree_rotate_left(t, sibling_node);
          sibling_node = fixup_node->parent->left; // update sibling after rotation
        }
        /* case 4*/
        sibling_node->color = fixup_node->parent->color;
        sibling_node->left->color = RBTREE_BLACK;
        sibling_node->parent->color = RBTREE_BLACK;
        rbtree_rotate_right(t, sibling_node->parent);
        fixup_node = t->root;
      }
    }
  }
  fixup_node->color = RBTREE_BLACK; // necessary?
}

int rbtree_erase(rbtree *t, node_t *p){
  if (NULL == p)
    return 0;
  // TODO: implement erase
  node_t *fixup_node, *reference_node, *remove_node = p;
  // remove_node : target to erase
  // reference_node : same with remove_node(1 child), or replacing remove_node(2 child). 
  /* reference_node's color is lost, and fixup will be executed if it's BLACK */
  // fixup_node:  node which replaces reference_node. will get extra_black. 
  /* if it's doubly_blacked, then needs fix-up */
  reference_node = remove_node;
  color_t reference_node_original_color = reference_node->color;
  /* p got only 1 child  */
  if (t->nil == remove_node->left)
  { // right child only
    fixup_node = remove_node->right; // remove_node->right will replace reference_node
    rbtree_transplant(t, remove_node, remove_node->right); // transplant rightChild
  }
  else if (t->nil == remove_node->right)
  {
    fixup_node = remove_node->left; // remove_node->left will replace reference_node
    rbtree_transplant(t, remove_node, remove_node->left); // transplant leftChild
  }
  else
  {
    // both child exists
    /* find "TREE-MINIMUM(p->right)", instead of using Function */
    reference_node = remove_node->right;
    while(t->nil != reference_node->left)
      reference_node = reference_node->left;
    // reference_node is the successor of remove_node p  
    reference_node_original_color = reference_node->color;
    fixup_node = reference_node->right; // reference_node->right will replace reference_node
    if (reference_node->parent == remove_node) // if reference_node == remove_node's rightChild
      fixup_node->parent = reference_node; // needed for special case when reference_node->right == NIL node
    else{ // if reference_node != remove_node's rightChild
      rbtree_transplant(t, reference_node, reference_node->right); // reference_node->right transplant (replaces reference_node)
      reference_node->right = remove_node->right; // reference_node replaces remove_node (attach rightSubtree)
      reference_node->right->parent = reference_node; // remove_node->right's new parent will be reference_node (interconnection completed)
    }
    rbtree_transplant(t, remove_node, reference_node); // reference_node transplant (replaces remove_node)
    reference_node->left = remove_node->left; // reference_node replaces remove_node (attach leftSubtree)
    reference_node->left->parent = reference_node; // remove_node->left's new parent will be reference_node (interconnection completed)
    reference_node->color = remove_node->color; // success remove_node's color
  }
  if (reference_node_original_color == RBTREE_BLACK) // lost color = reference_node_original_color !
    rbtree_erase_fixup(t,fixup_node); // fixup_node is doubly blacked ! needs to fix it up
  free(p);
  return 0;
}

int rbtree_inorder(const rbtree *t, node_t *p, key_t *arr, const size_t n, size_t *i){
  if (t->nil != p){
    rbtree_inorder(t, p->left, arr, n, i);
    if ((*i) < n)
      arr[(*i)++] = p->key; // if tree's bigger than n, return 0~n-1
    rbtree_inorder(t, p->right, arr, n, i);
  }
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n){
  // TODO: implement to_array // inorder
  node_t *root_node = t->root;
  size_t i = 0;
  rbtree_inorder(t, root_node, arr, n, &i);
  return 0;
}
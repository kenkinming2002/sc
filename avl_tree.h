// Before including this header, the following should be defined:
//  - SC_AVL_TREE_PREFIX
//  - SC_AVL_TREE_NODE_TYPE
//  - SC_AVL_TREE_KEY_TYPE
//
// SC_AVL_TREE_NODE_TYPE:
//   must at minimal have the same fields as the following struct:
//    - struct SC_AVL_TREE_NODE_TYPE *left
//    - struct SC_AVL_TREE_NODE_TYPE *right
//    - int                           bf

/*****************
 * Helper Macros *
 *****************/
#define SC_AVL_TREE_CONCAT2_INNER(a, b) a ## b
#define SC_AVL_TREE_CONCAT2(a, b) SC_AVL_TREE_CONCAT2_INNER(a, b)

#define SC_AVL_TREE_CONCAT3_INNER(a, b, c) a ## b ## c
#define SC_AVL_TREE_CONCAT3(a, b, c) SC_AVL_TREE_CONCAT3_INNER(a, b, c)

#define SC_AVL_TREE_MAKE_NAME(name)          SC_AVL_TREE_CONCAT3(SC_AVL_TREE_PREFIX, _, name)
#define SC_AVL_TREE_MAKE_NAME_INTERNAL(name) SC_AVL_TREE_CONCAT2(_, SC_AVL_TREE_MAKE_NAME(name))

/*********
 * Hooks *
 *********/
SC_AVL_TREE_KEY_TYPE SC_AVL_TREE_MAKE_NAME(key)(SC_AVL_TREE_NODE_TYPE *node);
int SC_AVL_TREE_MAKE_NAME(key_compare)(SC_AVL_TREE_KEY_TYPE key1, SC_AVL_TREE_KEY_TYPE key2);
void SC_AVL_TREE_MAKE_NAME(dispose)(SC_AVL_TREE_NODE_TYPE *node);

/*********
 * Types *
 *********/
struct SC_AVL_TREE_MAKE_NAME(avl_tree)
{
  SC_AVL_TREE_NODE_TYPE *root;
};

/*************
 * Functions *
 *************/
void SC_AVL_TREE_MAKE_NAME(avl_tree_init)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree);
void SC_AVL_TREE_MAKE_NAME(avl_tree_check)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree);
int SC_AVL_TREE_MAKE_NAME(avl_tree_insert)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE *node);
SC_AVL_TREE_NODE_TYPE *SC_AVL_TREE_MAKE_NAME(avl_tree_lookup)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_KEY_TYPE key);
SC_AVL_TREE_NODE_TYPE *SC_AVL_TREE_MAKE_NAME(avl_tree_remove)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_KEY_TYPE key);
void SC_AVL_TREE_MAKE_NAME(avl_tree_dispose)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree);

/*******************
 * Implementations *
 *******************/
#ifdef SC_AVL_TREE_IMPLEMENTATION
#include <assert.h>
#include <stddef.h>

#define SC_AVL_TREE_ERROR 0xdeadbeef
#define SC_AVL_TREE_MAX(a, b) ((a) > (b) ? (a) : (b))

void SC_AVL_TREE_MAKE_NAME(avl_tree_init)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree)
{
  avl_tree->root = NULL;
}

static unsigned SC_AVL_TREE_MAKE_NAME(avl_tree_node_height)(SC_AVL_TREE_NODE_TYPE *node)
{
  if(!node)
    return 0;

  unsigned height_left  = SC_AVL_TREE_MAKE_NAME(avl_tree_node_height)(node->left);
  unsigned height_right = SC_AVL_TREE_MAKE_NAME(avl_tree_node_height)(node->right);
  unsigned height = height_left > height_right ? height_left : height_right;
  return height + 1;
}

static void SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_check)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE *node)
{
  if(!node)
    return;

  if(node->left)  assert(SC_AVL_TREE_MAKE_NAME(key_compare)(SC_AVL_TREE_MAKE_NAME(key)(node->left),  SC_AVL_TREE_MAKE_NAME(key)(node)) < 0);
  if(node->right) assert(SC_AVL_TREE_MAKE_NAME(key_compare)(SC_AVL_TREE_MAKE_NAME(key)(node->right), SC_AVL_TREE_MAKE_NAME(key)(node)) > 0);

  int height_left  = SC_AVL_TREE_MAKE_NAME(avl_tree_node_height)(node->left);
  int height_right = SC_AVL_TREE_MAKE_NAME(avl_tree_node_height)(node->right);
  assert(height_left - height_right == node->bf);

  SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_check)(avl_tree, node->left);
  SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_check)(avl_tree, node->right);
}

void SC_AVL_TREE_MAKE_NAME(avl_tree_check)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree)
{
  SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_check)(avl_tree, avl_tree->root);
}

// Return change in subtree height
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_right)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root)
{
  SC_AVL_TREE_NODE_TYPE *node       = *root;
  SC_AVL_TREE_NODE_TYPE *child      = node->left;
  SC_AVL_TREE_NODE_TYPE *grandchild = child->right;

  assert(node->bf  == 2);
  assert(child->bf >= 0);

  *root        = child;
  node->left   = grandchild;
  child->right = node;

  if(child->bf == 1)
  {
    node->bf  = 0;
    child->bf = 0;
    return -1;
  }
  else if(child->bf == 0)
  {
    node->bf  = 1;
    child->bf = -1;
    return 0;
  }
  else
    assert(0 && "Unexpected branching factor");
}

// Return change in subtree height
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_left)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root)
{
  SC_AVL_TREE_NODE_TYPE *node       = *root;
  SC_AVL_TREE_NODE_TYPE *child      = node->right;
  SC_AVL_TREE_NODE_TYPE *grandchild = child->left;

  assert(node->bf  == -2);
  assert(child->bf <= 0);

  *root       = child;
  node->right = grandchild;
  child->left = node;

  if(child->bf == -1)
  {
    node->bf  = 0;
    child->bf = 0;
    return -1;
  }
  else if(child->bf == 0)
  {
    node->bf  = -1;
    child->bf = 1;
    return 0;
  }
  else
    assert(0 && "Unexpected branching factor");
}

// Return change in subtree height
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_left_right)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root)
{
  SC_AVL_TREE_NODE_TYPE *node                   = *root;
  SC_AVL_TREE_NODE_TYPE *child                  = node->left;
  SC_AVL_TREE_NODE_TYPE *grandchild             = child->right;
  SC_AVL_TREE_NODE_TYPE *great_grandchild_left  = grandchild->left;
  SC_AVL_TREE_NODE_TYPE *great_grandchild_right = grandchild->right;

  assert(node->bf  == 2);
  assert(child->bf == -1);

  *root             = grandchild;
  grandchild->left  = child;
  grandchild->right = node;
  node->left        = great_grandchild_right;
  child->right      = great_grandchild_left;

  if(grandchild->bf == 1)
  {
    node->bf  = -1;
    child->bf = 0;
  }
  else if(grandchild->bf == -1)
  {
    node->bf  = 0;
    child->bf = 1;
  }
  else
  {
    node->bf  = 0;
    child->bf = 0;
  }
  grandchild->bf = 0;
  return -1;
}

// Return change in subtree height
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_right_left)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root)
{
  SC_AVL_TREE_NODE_TYPE *node                   = *root;
  SC_AVL_TREE_NODE_TYPE *child                  = node->right;
  SC_AVL_TREE_NODE_TYPE *grandchild             = child->left;
  SC_AVL_TREE_NODE_TYPE *great_grandchild_left  = grandchild->left;
  SC_AVL_TREE_NODE_TYPE *great_grandchild_right = grandchild->right;

  assert(node->bf  == -2);
  assert(child->bf == 1);

  *root             = grandchild;
  grandchild->left  = node;
  grandchild->right = child;
  node->right       = great_grandchild_left;
  child->left       = great_grandchild_right;

  if(grandchild->bf == 1)
  {
    node->bf  = 0;
    child->bf = -1;
  }
  else if(grandchild->bf == -1)
  {
    node->bf  = 1;
    child->bf = 0;
  }
  else
  {
    node->bf  = 0;
    child->bf = 0;
  }
  grandchild->bf = 0;
  return -1;
}

// Return change in tree height at root
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_bf)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root)
{
  switch((*root)->bf)
  {
  case -1:
  case 0:
  case 1:
    return 0;
  case -2:
    if((*root)->right->bf <= 0)
      return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_left)(avl_tree, root);
    else
      return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_right_left)(avl_tree, root);
  case 2:
    if((*root)->left->bf >= 0)
      return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_right)(avl_tree, root);
    else
      return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_rotate_left_right)(avl_tree, root);
  default:
    assert(0 && "Unreachable");
  }
}

// Return change in tree height at root
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_left_subtree)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, int change)
{
  int change_bf = change;
  int change_height;
  switch((*root)->bf)
  {
  case  0: change_height = SC_AVL_TREE_MAX(change, 0); break;
  case -1: change_height = 0;                       break;
  case +1: change_height = change;                  break;
  }
  (*root)->bf += change_bf;
  return change_height + SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_bf)(avl_tree, root);
}

// Return change in tree height at root
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_right_subtree)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, int change)
{
  int change_bf = -change;
  int change_height;
  switch((*root)->bf)
  {
  case  0: change_height = SC_AVL_TREE_MAX(change, 0); break;
  case +1: change_height = 0;                       break;
  case -1: change_height = change;                  break;
  }
  (*root)->bf += change_bf;
  return change_height + SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_bf)(avl_tree, root);
}

// Return change in subtree height or SC_AVL_TREE_ERROR on error
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_insert)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, SC_AVL_TREE_NODE_TYPE *node)
{
  if(!(*root))
  {
    node->left  = NULL;
    node->right = NULL;
    node->bf    = 0;
    *root = node;
    return 1;
  }

  int result = SC_AVL_TREE_MAKE_NAME(key_compare)(SC_AVL_TREE_MAKE_NAME(key)(node), SC_AVL_TREE_MAKE_NAME(key)(*root));
  if(result < 0)
  {
    int result = SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_insert)(avl_tree, &(*root)->left, node);
    if(result == SC_AVL_TREE_ERROR)
      return SC_AVL_TREE_ERROR;
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_left_subtree)(avl_tree, root, result);
  }
  else if(result > 0)
  {
    int result = SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_insert)(avl_tree, &(*root)->right, node);
    if(result == SC_AVL_TREE_ERROR)
      return SC_AVL_TREE_ERROR;
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_right_subtree)(avl_tree, root, result);
  }
  else
    return SC_AVL_TREE_ERROR;
}

int SC_AVL_TREE_MAKE_NAME(avl_tree_insert)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE *node)
{
  return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_insert)(avl_tree, &avl_tree->root, node) != SC_AVL_TREE_ERROR;
}

static SC_AVL_TREE_NODE_TYPE *SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_lookup)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE *node, SC_AVL_TREE_KEY_TYPE key)
{
  if(!node)
    return NULL;

  int result = SC_AVL_TREE_MAKE_NAME(key_compare)(key, SC_AVL_TREE_MAKE_NAME(key)(node));
  if(result < 0)
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_lookup)(avl_tree, node->left, key);
  else if(result > 0)
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_lookup)(avl_tree, node->right, key);
  else
    return node;
}

SC_AVL_TREE_NODE_TYPE *SC_AVL_TREE_MAKE_NAME(avl_tree_lookup)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_KEY_TYPE key)
{
  return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_lookup)(avl_tree, avl_tree->root, key);
}

static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node_min)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, SC_AVL_TREE_NODE_TYPE **node);
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, SC_AVL_TREE_NODE_TYPE **node);

// Return change in tree height at root
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node_min)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, SC_AVL_TREE_NODE_TYPE **node)
{
  if(!(*root)->left)
  {
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node)(avl_tree, root, node);
  }

  int result = SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node_min)(avl_tree, &(*root)->left, node);
  return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_left_subtree)(avl_tree, root, result);
}

// Return change in tree height at root
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, SC_AVL_TREE_NODE_TYPE **node)
{
  if(!(*root)->left && !(*root)->right)
  {
    *node = *root;
    *root = NULL;
    return -1;
  }
  else if((*root)->left && !(*root)->right)
  {
    *node = *root;
    *root = (*root)->left;
    return -1;
  }
  else if(!(*root)->left && (*root)->right)
  {
    *node = *root;
    *root = (*root)->right;
    return -1;
  }
  else
  {
    SC_AVL_TREE_NODE_TYPE *victim = NULL;

    int result = SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node_min)(avl_tree, &(*root)->right, &victim);
    victim->left  = (*root)->left;
    victim->right = (*root)->right;
    victim->bf    = (*root)->bf;

    *node = *root;
    *root = victim;

    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_right_subtree)(avl_tree, root, result);
  }
}

// Return change in tree height at root or SC_AVL_TREE_ERROR on error
static int SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE **root, SC_AVL_TREE_NODE_TYPE **node, SC_AVL_TREE_KEY_TYPE key)
{
  if(!(*root))
    return SC_AVL_TREE_ERROR;

  int change_bf;
  int change_height;

  int result = SC_AVL_TREE_MAKE_NAME(key_compare)(key, SC_AVL_TREE_MAKE_NAME(key)(*root));
  if(result < 0)
  {
    int result = SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove)(avl_tree, &(*root)->left, node, key);
    if(result == SC_AVL_TREE_ERROR)
      return SC_AVL_TREE_ERROR;
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_left_subtree)(avl_tree, root, result);
  }
  else if(result > 0)
  {
    int result = SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove)(avl_tree, &(*root)->right, node, key);
    if(result == SC_AVL_TREE_ERROR)
      return SC_AVL_TREE_ERROR;
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_right_subtree)(avl_tree, root, result);
  }
  else
    return SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove_node)(avl_tree, root, node);

  (*root)->bf += change_bf;
  return change_height + SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_update_bf)(avl_tree, root);
}

SC_AVL_TREE_NODE_TYPE *SC_AVL_TREE_MAKE_NAME(avl_tree_remove)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_KEY_TYPE key)
{
  SC_AVL_TREE_NODE_TYPE *node = NULL;
  SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_remove)(avl_tree, &avl_tree->root, &node, key);
  return node;
}

static void SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_dispose)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree, SC_AVL_TREE_NODE_TYPE *root)
{
  if(root)
  {
    SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_dispose)(avl_tree, root->left);
    SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_dispose)(avl_tree, root->right);
    SC_AVL_TREE_MAKE_NAME(dispose)(root);
  }
}

void SC_AVL_TREE_MAKE_NAME(avl_tree_dispose)(struct SC_AVL_TREE_MAKE_NAME(avl_tree) *avl_tree)
{
  SC_AVL_TREE_MAKE_NAME_INTERNAL(avl_tree_dispose)(avl_tree, avl_tree->root);
}

#endif

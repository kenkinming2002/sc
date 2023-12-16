#include "avl_tree.h"

#include <assert.h>
#include <limits.h>

#define AVL_TREE_ERROR 0xdeadbeef
#define AVL_TREE_MAX(a, b) ((a) > (b) ? (a) : (b))

static unsigned avl_tree_node_height(struct avl_tree_node *node)
{
  if(!node)
    return 0;

  unsigned height_left  = avl_tree_node_height(node->left);
  unsigned height_right = avl_tree_node_height(node->right);
  unsigned height = height_left > height_right ? height_left : height_right;
  return height + 1;
}

static void _avl_tree_check(struct avl_tree *avl_tree, struct avl_tree_node *node)
{
  if(!node)
    return;

  if(node->left)  assert(avl_tree->ops->compare(avl_tree->ops->key(node->left),  avl_tree->ops->key(node)) < 0);
  if(node->right) assert(avl_tree->ops->compare(avl_tree->ops->key(node->right), avl_tree->ops->key(node)) > 0);

  int height_left  = avl_tree_node_height(node->left);
  int height_right = avl_tree_node_height(node->right);
  assert(height_left - height_right == node->bf);

  _avl_tree_check(avl_tree, node->left);
  _avl_tree_check(avl_tree, node->right);
}

void avl_tree_check(struct avl_tree *avl_tree)
{
  _avl_tree_check(avl_tree, avl_tree->root);
}

// Return change in subtree height
static int avl_tree_rotate_right(struct avl_tree *avl_tree, struct avl_tree_node **root)
{
  struct avl_tree_node *node       = *root;
  struct avl_tree_node *child      = node->left;
  struct avl_tree_node *grandchild = child->right;

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
static int avl_tree_rotate_left(struct avl_tree *avl_tree, struct avl_tree_node **root)
{
  struct avl_tree_node *node       = *root;
  struct avl_tree_node *child      = node->right;
  struct avl_tree_node *grandchild = child->left;

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
static int avl_tree_rotate_left_right(struct avl_tree *avl_tree, struct avl_tree_node **root)
{
  struct avl_tree_node *node                   = *root;
  struct avl_tree_node *child                  = node->left;
  struct avl_tree_node *grandchild             = child->right;
  struct avl_tree_node *great_grandchild_left  = grandchild->left;
  struct avl_tree_node *great_grandchild_right = grandchild->right;

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
static int avl_tree_rotate_right_left(struct avl_tree *avl_tree, struct avl_tree_node **root)
{
  struct avl_tree_node *node                   = *root;
  struct avl_tree_node *child                  = node->right;
  struct avl_tree_node *grandchild             = child->left;
  struct avl_tree_node *great_grandchild_left  = grandchild->left;
  struct avl_tree_node *great_grandchild_right = grandchild->right;

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
static int avl_tree_update_bf(struct avl_tree *avl_tree, struct avl_tree_node **root)
{
  switch((*root)->bf)
  {
  case -1:
  case 0:
  case 1:
    return 0;
  case -2:
    if((*root)->right->bf <= 0)
      return avl_tree_rotate_left(avl_tree, root);
    else
      return avl_tree_rotate_right_left(avl_tree, root);
  case 2:
    if((*root)->left->bf >= 0)
      return avl_tree_rotate_right(avl_tree, root);
    else
      return avl_tree_rotate_left_right(avl_tree, root);
  default:
    assert(0 && "Unreachable");
  }
}

// Return change in tree height at root
static int avl_tree_update_left_subtree(struct avl_tree *avl_tree, struct avl_tree_node **root, int change)
{
  int change_bf = change;
  int change_height;
  switch((*root)->bf)
  {
  case  0: change_height = AVL_TREE_MAX(change, 0); break;
  case -1: change_height = 0;                       break;
  case +1: change_height = change;                  break;
  }
  (*root)->bf += change_bf;
  return change_height + avl_tree_update_bf(avl_tree, root);
}

// Return change in tree height at root
static int avl_tree_update_right_subtree(struct avl_tree *avl_tree, struct avl_tree_node **root, int change)
{
  int change_bf = -change;
  int change_height;
  switch((*root)->bf)
  {
  case  0: change_height = AVL_TREE_MAX(change, 0); break;
  case +1: change_height = 0;                       break;
  case -1: change_height = change;                  break;
  }
  (*root)->bf += change_bf;
  return change_height + avl_tree_update_bf(avl_tree, root);
}

// Return change in subtree height or AVL_TREE_ERROR on error
static int _avl_tree_insert(struct avl_tree *avl_tree, struct avl_tree_node **root, struct avl_tree_node *node)
{
  if(!(*root))
  {
    node->left  = NULL;
    node->right = NULL;
    node->bf    = 0;
    *root = node;
    return 1;
  }

  int result = avl_tree->ops->compare(avl_tree->ops->key(node), avl_tree->ops->key(*root));
  if(result < 0)
  {
    int result = _avl_tree_insert(avl_tree, &(*root)->left, node);
    if(result == AVL_TREE_ERROR)
      return AVL_TREE_ERROR;
    return avl_tree_update_left_subtree(avl_tree, root, result);
  }
  else if(result > 0)
  {
    int result = _avl_tree_insert(avl_tree, &(*root)->right, node);
    if(result == AVL_TREE_ERROR)
      return AVL_TREE_ERROR;
    return avl_tree_update_right_subtree(avl_tree, root, result);
  }
  else
    return AVL_TREE_ERROR;
}

int avl_tree_insert(struct avl_tree *avl_tree, struct avl_tree_node *node)
{
  return _avl_tree_insert(avl_tree, &avl_tree->root, node) != AVL_TREE_ERROR;
}

static struct avl_tree_node *_avl_tree_lookup(struct avl_tree *avl_tree, struct avl_tree_node *node, avl_tree_key_t key)
{
  if(!node)
    return NULL;

  int result = avl_tree->ops->compare(key, avl_tree->ops->key(node));
  if(result < 0)
    return _avl_tree_lookup(avl_tree, node->left, key);
  else if(result > 0)
    return _avl_tree_lookup(avl_tree, node->right, key);
  else
    return node;
}

struct avl_tree_node *avl_tree_lookup(struct avl_tree *avl_tree, avl_tree_key_t key)
{
  return _avl_tree_lookup(avl_tree, avl_tree->root, key);
}

static int _avl_tree_remove_node_min(struct avl_tree *avl_tree, struct avl_tree_node **root, struct avl_tree_node **node);
static int _avl_tree_remove_node(struct avl_tree *avl_tree, struct avl_tree_node **root, struct avl_tree_node **node);

// Return change in tree height at root
static int _avl_tree_remove_node_min(struct avl_tree *avl_tree, struct avl_tree_node **root, struct avl_tree_node **node)
{
  if(!(*root)->left)
  {
    return _avl_tree_remove_node(avl_tree, root, node);
  }

  int result = _avl_tree_remove_node_min(avl_tree, &(*root)->left, node);
  return avl_tree_update_left_subtree(avl_tree, root, result);
}

// Return change in tree height at root
static int _avl_tree_remove_node(struct avl_tree *avl_tree, struct avl_tree_node **root, struct avl_tree_node **node)
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
    struct avl_tree_node *victim = NULL;

    int result = _avl_tree_remove_node_min(avl_tree, &(*root)->right, &victim);
    victim->left  = (*root)->left;
    victim->right = (*root)->right;
    victim->bf    = (*root)->bf;

    *node = *root;
    *root = victim;

    return avl_tree_update_right_subtree(avl_tree, root, result);
  }
}

// Return change in tree height at root or AVL_TREE_ERROR on error
static int _avl_tree_remove(struct avl_tree *avl_tree, struct avl_tree_node **root, struct avl_tree_node **node, avl_tree_key_t key)
{
  if(!(*root))
    return AVL_TREE_ERROR;

  int change_bf;
  int change_height;

  int result = avl_tree->ops->compare(key, avl_tree->ops->key(*root));
  if(result < 0)
  {
    int result = _avl_tree_remove(avl_tree, &(*root)->left, node, key);
    if(result == AVL_TREE_ERROR)
      return AVL_TREE_ERROR;
    return avl_tree_update_left_subtree(avl_tree, root, result);
  }
  else if(result > 0)
  {
    int result = _avl_tree_remove(avl_tree, &(*root)->right, node, key);
    if(result == AVL_TREE_ERROR)
      return AVL_TREE_ERROR;
    return avl_tree_update_right_subtree(avl_tree, root, result);
  }
  else
    return _avl_tree_remove_node(avl_tree, root, node);

  (*root)->bf += change_bf;
  return change_height + avl_tree_update_bf(avl_tree, root);
}

struct avl_tree_node *avl_tree_remove(struct avl_tree *avl_tree, avl_tree_key_t key)
{
  struct avl_tree_node *node = NULL;
  _avl_tree_remove(avl_tree, &avl_tree->root, &node, key);
  return node;
}

static void _avl_tree_dispose(struct avl_tree *avl_tree, struct avl_tree_node *root)
{
  if(root)
  {
    _avl_tree_dispose(avl_tree, root->left);
    _avl_tree_dispose(avl_tree, root->right);
    avl_tree->ops->dispose(root);
  }
}

void avl_tree_dispose(struct avl_tree *avl_tree)
{
  _avl_tree_dispose(avl_tree, avl_tree->root);
}


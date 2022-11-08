/*************************************
* Lab 1 Exercise 2
* Name: Nguyen Minh Nguyen 
* Student No: A0200786E
* Lab Group: B07
*************************************/

#include "node.h"
#include <stdlib.h>
#include <stdio.h>


// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit 


// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
    node* cur_ptr = lst->head;
    node* newNode = (node*)malloc(sizeof(node));
    newNode->data = data;
    // Insert at the start of the linked list
    if (index == 0){ 
        newNode->next = cur_ptr;
        lst->head = newNode;
        return;
    }
    while (--index > 0){
        cur_ptr = cur_ptr->next;
    }
    // Add node at the end of the linked list
    if (cur_ptr->next == NULL){
        newNode->next = NULL;
        cur_ptr->next = newNode;   
    }
    // Else add a new node in the middle of the list
    else{
        newNode->next = cur_ptr->next;
        cur_ptr->next = newNode;
    }
    return;

}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
    node* cur_ptr = lst->head;
    if (index == 0){
        lst->head = cur_ptr->next;
        free(cur_ptr);
        return;
    }
    while (--index > 0){
        cur_ptr = cur_ptr->next;
    }
    // Delete node at the end of the linked list
    if (cur_ptr->next == NULL){
        node* delNode = cur_ptr->next;
        cur_ptr->next = NULL;
        free(delNode);
    }
    // Else delete a new node in the middle of the list
    else{
        node* delNode = cur_ptr->next;
        cur_ptr->next = delNode->next;
        free(delNode);
    }
    return;
}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.
//Printing of the index is already handled in ex2.c
int search_list(list *lst, int element) {
    int index = 0;
    node* curNode = lst->head;
    if (curNode == NULL) return -2;
    while (curNode!=NULL){
        if (curNode->data == element) return index;
        curNode = curNode->next;
        index++;
    }
    return -1;
}

// Reverses the list with the last node becoming the first node.
void reverse_list(list *lst) {
    node* curNode = lst->head;
    node* prevNode = NULL, *nextNode = NULL;
    while (curNode != NULL){
        nextNode = curNode->next;
        curNode->next = prevNode;
        prevNode = curNode;
        curNode = nextNode;
    }
    lst->head = prevNode;
    return;
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
    node* curNode = lst->head;
    node* tmpNode = NULL;
    while (curNode != NULL){
        tmpNode = curNode;
        curNode = curNode->next;
        free(tmpNode);
    }
    lst->head = NULL;
    return;
}

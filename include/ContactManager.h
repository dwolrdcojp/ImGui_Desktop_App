#pragma once
#include <iostream>
#include <vector>
#include "Contact.h"

class ContactManager {
public:
  std::vector<Contact*> m_contacts;
  
  ContactManager();

  void addContact(Contact* contact);
};

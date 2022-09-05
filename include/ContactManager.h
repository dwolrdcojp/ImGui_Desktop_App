#pragma once
#include <iostream>
#include <vector>
#include "Contact.h"
#include <string>
#include <map>

struct ContactConfig { std::string F, L, P, E; };

class ContactManager {
public:

  std::vector<Contact*> m_contacts;
  std::map<std::string, Contact*> m_contactBook;
  ContactConfig m_config;
  
  ContactManager();

  void addContact(Contact* contact);

  Contact* findContact(std::string first_name);

  void removeContact(Contact* contact);

  void init(const std::string& contacts);
};

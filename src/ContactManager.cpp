#include "../include/ContactManager.h"

ContactManager::ContactManager()
{
  std::cout << "ContactManager constructor..." << std::endl;
}

void ContactManager::addContact(Contact* contact)
{

  m_contacts.push_back(contact);

}

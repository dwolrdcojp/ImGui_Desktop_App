#include "../include/ContactManager.h" 
#include <fstream>

ContactManager::ContactManager()
{
  std::cout << "ContactManager constructor..." << std::endl;
}

void ContactManager::addContact(Contact* contact)
{

  m_contacts.push_back(contact);
  m_contactBook[contact->m_first_name] = contact;
}

void ContactManager::init(const std::string& contacts)
{
  std::ifstream fin(contacts);
  std::vector<ContactConfig> cConfig;


  if(fin.is_open())
  {
    while(fin >> m_config.F)
    {
      fin >> m_config.L >> m_config.P >> m_config.E;
      cConfig.push_back(m_config);
    }
  }

  for(auto c : cConfig)
  {
    Contact* contact = new Contact(c.F, c.L, c.P, c.E);
    addContact(contact);
  }
}

Contact* ContactManager::findContact(std::string first_name)
{
  auto found = m_contactBook.find(first_name);
  if(found != m_contactBook.end())
  {
    return found->second;
  }
  else 
  {
    Contact* contact = new Contact("", "", "", "");
    return contact;
  }
}

void ContactManager::removeContact(Contact* contact)
{

  for(auto it = m_contacts.begin(); it != m_contacts.end(); )
  {
    if(*it == contact)
    {
      it = m_contacts.erase(it);
    }
    else 
    {
      it++;
    }
  }

  for(auto it = m_contactBook.begin(); it != m_contactBook.end(); )
  {
    if(it->second == contact)
    {
      it = m_contactBook.erase(it);
    }
    else 
    {
      it++;
    }
  }
}

int ContactManager::save(const std::string& contacts)
{
  std::ofstream myfile(contacts);
  if (myfile.is_open())
  {
    for(auto c : m_contacts)
    {
      myfile << c->m_first_name << ' '<< c->m_last_name << ' ' << c->m_phone << ' ' << c->m_email << std::endl;
    }
    myfile.close();
    return 1;
  }
  else  
  {
    std::cout << "Unable to open file" << std::endl;
    return 0;
  }
}


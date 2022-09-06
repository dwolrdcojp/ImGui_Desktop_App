#include "../include/Contact.h"

Contact::Contact() 
{
  std::cout << "Contact constructor..." << std::endl;
};

void Contact::updateContact(std::string first, std::string last, std::string phone, std::string email)
{
  m_first_name = first;
  m_last_name = last;
  m_phone = phone;
  m_email = email;
}

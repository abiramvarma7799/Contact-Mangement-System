#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

struct Contact {
    string name;
    string phone;
    string email;
    Contact* next;

    Contact(string n, string p, string e) {
        name = n;
        phone = p;
        email = e;
        next = nullptr;
    }
};

class ContactManager {
private:
    Contact* head;

    static const int TABLE_SIZE = 101;

    struct HashNode {
        string phone;
        Contact* contact;
        HashNode* next;

        HashNode(string p, Contact* c) {
            phone = p;
            contact = c;
            next = nullptr;
        }
    };

    HashNode* hashTable[TABLE_SIZE];

    int hashFunction(string phone) {
        unsigned long long hash = 0;

        for (char c : phone) {
            if (c >= '0' && c <= '9')
                hash = (hash * 31 + (c - '0')) % TABLE_SIZE;
        }

        return hash % TABLE_SIZE;
    }

    Contact* merge(Contact* a, Contact* b) {
        if (!a)
            return b;

        if (!b)
            return a;

        if (a->name <= b->name) {
            a->next = merge(a->next, b);
            return a;
        } else {
            b->next = merge(a, b->next);
            return b;
        }
    }

    void split(Contact* source, Contact*& front, Contact*& back) {
        Contact* slow = source;
        Contact* fast = source->next;

        while (fast) {
            fast = fast->next;

            if (fast) {
                slow = slow->next;
                fast = fast->next;
            }
        }

        front = source;
        back = slow->next;
        slow->next = nullptr;
    }

    Contact* mergeSort(Contact* node) {
        if (!node || !node->next)
            return node;

        Contact* front;
        Contact* back;

        split(node, front, back);

        front = mergeSort(front);
        back = mergeSort(back);

        return merge(front, back);
    }

    void addToHashTable(Contact* contact) {
        int index = hashFunction(contact->phone);

        HashNode* newNode =
            new HashNode(contact->phone, contact);

        newNode->next = hashTable[index];
        hashTable[index] = newNode;
    }

    void removeFromHashTable(string phone) {
        int index = hashFunction(phone);

        HashNode* current = hashTable[index];
        HashNode* previous = nullptr;

        while (current) {
            if (current->phone == phone) {

                if (previous)
                    previous->next = current->next;
                else
                    hashTable[index] = current->next;

                delete current;
                return;
            }

            previous = current;
            current = current->next;
        }
    }

public:
    ContactManager() {
        head = nullptr;

        for (int i = 0; i < TABLE_SIZE; i++)
            hashTable[i] = nullptr;

        loadFromFile();
    }

    ~ContactManager() {
        saveToFile();

        Contact* current = head;

        while (current) {
            Contact* temp = current;
            current = current->next;
            delete temp;
        }

        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* currentHash = hashTable[i];

            while (currentHash) {
                HashNode* temp = currentHash;
                currentHash = currentHash->next;
                delete temp;
            }
        }
    }

    bool phoneExists(string phone) {
        int index = hashFunction(phone);

        HashNode* current = hashTable[index];

        while (current) {
            if (current->phone == phone)
                return true;

            current = current->next;
        }

        return false;
    }

    void addContact() {
        string name, phone, email;

        cin.ignore();

        cout << "\nEnter Name: ";
        getline(cin, name);

        cout << "Enter Phone: ";
        getline(cin, phone);

        if (phoneExists(phone)) {
            cout << "\nContact with this phone number already exists.\n";
            return;
        }

        cout << "Enter Email: ";
        getline(cin, email);

        Contact* newContact =
            new Contact(name, phone, email);

        if (!head) {
            head = newContact;
        } else {
            Contact* current = head;

            while (current->next)
                current = current->next;

            current->next = newContact;
        }

        addToHashTable(newContact);

        cout << "\nContact added successfully!\n";
    }

    void displayContacts() {
        if (!head) {
            cout << "\nNo contacts available.\n";
            return;
        }

        cout << "\n================ CONTACT LIST ================\n";

        cout << left
             << setw(25) << "Name"
             << setw(18) << "Phone"
             << setw(30) << "Email" << endl;

        cout << string(73, '-') << endl;

        Contact* current = head;

        while (current) {
            cout << left
                 << setw(25) << current->name
                 << setw(18) << current->phone
                 << setw(30) << current->email
                 << endl;

            current = current->next;
        }
    }

    void searchByName() {
        string name;

        cin.ignore();

        cout << "\nEnter name to search: ";
        getline(cin, name);

        Contact* current = head;
        bool found = false;

        while (current) {

            if (current->name == name) {
                cout << "\nContact Found\n";
                cout << "Name  : " << current->name << endl;
                cout << "Phone : " << current->phone << endl;
                cout << "Email : " << current->email << endl;

                found = true;
                break;
            }

            current = current->next;
        }

        if (!found)
            cout << "\nContact not found.\n";
    }

    void searchByPhone() {
        string phone;

        cin.ignore();

        cout << "\nEnter phone number: ";
        getline(cin, phone);

        int index = hashFunction(phone);

        HashNode* current = hashTable[index];

        while (current) {

            if (current->phone == phone) {
                Contact* c = current->contact;

                cout << "\nContact Found\n";
                cout << "Name  : " << c->name << endl;
                cout << "Phone : " << c->phone << endl;
                cout << "Email : " << c->email << endl;

                return;
            }

            current = current->next;
        }

        cout << "\nContact not found.\n";
    }

    void updateContact() {
        string phone;

        cin.ignore();

        cout << "\nEnter phone number of contact: ";
        getline(cin, phone);

        int index = hashFunction(phone);

        HashNode* currentHash = hashTable[index];

        while (currentHash) {

            if (currentHash->phone == phone) {

                Contact* contact = currentHash->contact;

                cout << "\nCurrent Details\n";
                cout << "Name  : " << contact->name << endl;
                cout << "Phone : " << contact->phone << endl;
                cout << "Email : " << contact->email << endl;

                cout << "\nEnter new name: ";
                getline(cin, contact->name);

                cout << "Enter new email: ";
                getline(cin, contact->email);

                cout << "\nContact updated successfully!\n";

                return;
            }

            currentHash = currentHash->next;
        }

        cout << "\nContact not found.\n";
    }

    void deleteContact() {
        string phone;

        cin.ignore();

        cout << "\nEnter phone number to delete: ";
        getline(cin, phone);

        Contact* current = head;
        Contact* previous = nullptr;

        while (current) {

            if (current->phone == phone) {

                if (previous)
                    previous->next = current->next;
                else
                    head = current->next;

                removeFromHashTable(phone);

                delete current;

                cout << "\nContact deleted successfully!\n";

                return;
            }

            previous = current;
            current = current->next;
        }

        cout << "\nContact not found.\n";
    }

    void sortContacts() {
        if (!head || !head->next) {
            cout << "\nNot enough contacts to sort.\n";
            return;
        }

        head = mergeSort(head);

        cout << "\nContacts sorted alphabetically by name.\n";
    }

    void saveToFile() {
        ofstream file("contacts.txt");

        Contact* current = head;

        while (current) {
            file << current->name << "|"
                 << current->phone << "|"
                 << current->email << endl;

            current = current->next;
        }

        file.close();
    }

    void loadFromFile() {
        ifstream file("contacts.txt");

        if (!file)
            return;

        string name, phone, email;

        while (getline(file, name, '|') &&
               getline(file, phone, '|') &&
               getline(file, email)) {

            if (phoneExists(phone))
                continue;

            Contact* newContact =
                new Contact(name, phone, email);

            if (!head) {
                head = newContact;
            } else {
                Contact* current = head;

                while (current->next)
                    current = current->next;

                current->next = newContact;
            }

            addToHashTable(newContact);
        }

        file.close();
    }

    void menu() {
        int choice;

        do {
            cout << "\n\n============================================";
            cout << "\n       CONTACT MANAGEMENT SYSTEM";
            cout << "\n============================================";
            cout << "\n1. Add Contact";
            cout << "\n2. Display Contacts";
            cout << "\n3. Search Contact by Name";
            cout << "\n4. Search Contact by Phone";
            cout << "\n5. Update Contact";
            cout << "\n6. Delete Contact";
            cout << "\n7. Sort Contacts";
            cout << "\n8. Save Contacts";
            cout << "\n9. Exit";
            cout << "\n============================================";
            cout << "\nEnter your choice: ";

            cin >> choice;

            switch (choice) {

                case 1:
                    addContact();
                    break;

                case 2:
                    displayContacts();
                    break;

                case 3:
                    searchByName();
                    break;

                case 4:
                    searchByPhone();
                    break;

                case 5:
                    updateContact();
                    break;

                case 6:
                    deleteContact();
                    break;

                case 7:
                    sortContacts();
                    break;

                case 8:
                    saveToFile();
                    cout << "\nContacts saved successfully!\n";
                    break;

                case 9:
                    cout << "\nThank you for using Contact Management System!\n";
                    break;

                default:
                    cout << "\nInvalid choice. Try again.\n";
            }

        } while (choice != 9);
    }
};

int main() {

    ContactManager manager;

    manager.menu();

    return 0;
}

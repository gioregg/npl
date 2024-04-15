#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include<cstdint>
#include <unistd.h>
#include<sys/socket.h>
#include<vector>
#include<system_error>
#include "sockaddress.hpp"
namespace npl{

    typedef std::vector<uint8_t> buffer;// cosi buffer è il nome che do ad un array di caratteri(un carattere in C sono 8 bit)

    template<int F, int type> //il protocollo glielo passo quando lo creo
    class socket{
    //il file descriptor mi descrive il socket quindi è la variabile interna
    private:
        int _sockfd;
    public:
        explicit socket(int protocol=0)//perche gli altri gli ho passati con il template. mettendolo = 0 è come fosse un costruttore di default    
        {
            if((_sockfd = ::socket(F,type,protocol))==-1)//i due :: mi dice che fuori il namespace quindi chiama la socket() vera
            {//socket() è una system call, cioe te lo da il sistema operativo(è quella del C, perche il C++ non ha delle system call proprie
            //per le socket ed è anche per questo che si fa noi la classe socket)
                throw std::system_error(errno,std::generic_category(),"socket");//system call che lancia un eccezione. "socket" è il messaggio che viene lanciato.

            }
        }
        socket(const socket& rhs) = delete; //sia il copy che l assignment operator devono essere disabilitati, perche copiare
        socket& operator=(const socket&) = delete;//un socket in un altro non ha senso

        socket(socket&& rhs)//move operator
        : _sockfd(rhs._sockfd)
        {
            rhs._sockfd = -1;//non chiuderlo perche se lo chiudi , chiude anche l altro
        }

        socket& operator=(socket&& rhs)//move assigment operator
        {
            if (*this!=rhs)
            {
                
                this->close();//la implemento sotto|
                _sockfd = rhs._sockfd;
                rhs._sockfd = -1;
            }
            return *this;
        }

        void close()
        {
            if (_sockfd!=-1)
            {
                ::close(_sockfd);
                _sockfd = -1;
            }
        }

        void bind(const sockaddress<F>& addr)//la chiamo bind() come la systemcall tanto è dentro il mio namespace
        {
            //addr perche la bind vuole l indirizzo alla C.
            if((::bind(_sockfd, &addr.c_addr(), addr.len()))==-1){
                throw std::system_error(errno,std::system_category,"bind");
            }

        }

        void listen(int backlog=5)
        {
            //addr perche la bind vuole l indirizzo alla C.
            if(::lisen(_sockfd, backlog)==-1){
                throw std::system_error(errno,std::system_category,"listen");
            }

        }

        std::pair<socket, sockaddress<F> > accept()
        {
            sockaddress<F> peer;//mi creo un oggetto vuoto da passare. nella accept del C il sockaddr è bianco(= è vuoto)
            //nella accept del C ritorna l intero del file descriptor
            socket accepted;// è uno dei miei
            //uso & perche la accept vuole il puntatore
            if((accepted._sockfd=::accept(_sockfd, &peer.c_addr(), &peer.len()))==-1)
            {
                throw std::system_error(errno,std::system_category,"accept");

            }
            return std::make_pair(std::move(accepted), pair);//muovo accepted perche era non copiabile


        }

        void connect(const sockaddress<F>& remote)
        {
            //remote è l  indirizzo a cui voglio connettermi
            if((::connect(_sockfd, &remote.c_addr(), remote.len()))==-1){
                throw std::system_error(errno,std::system_category,"connect");
            }

        }

        //IO methods

        //CONNECTION
        //WRITE
        //slide 28
        //versione alla C
        //ptrdiff_t è la differenza tra i puntatori di inizio e fine
        //&buf perche vuole il primo indirizzo del buf.
        std::ptrdiff_t write(const buffer& buf) const
        {
            return ::write(_sockfd,&buf[0], buf.size())
            
        }
        
        //READ
        std::ptrdiff_t read(buffer& buf) const
        {
            return ::read(_sockfd,&buf[0], buf.size())

        }
        //read alla python
        buffer read(size_t n) const
        {
            buffer buf(n);
            int nbytes = ::read(_sockfd,&buf[0], buf.size());
            return buffer(buf.begin(),buf.begin()+nbytes);// perche cosi si ritorna un buffer di veramente quanti caratteri letti
            //magari n è maggiore di quanti byte leggiamo

        }

        //NO CONNECTION
        //remote è quello dove inviare questa roba e buf è dove c'è scritto la roba che invio
        std::ptrdiff_t sendto(const buffer& buf, const sockaddr<F>& remote, int flags=0) const
        {
            return ::sendto(_sockfd, &buf[0], buf.size(), flags, &remote.c_addr(), remote.len())
        }

        //buf è dove va a scrivere
        //remote è bianco, ci vado a mettere poi chi mi invia la roba
        std::ptrdiff_t recvfrom(buffer& buf, sockaddress<F>& remote, int flags==) const
        {
            //&remote.len perche ci devo scrivere. mentre prima era remote.len() perche li quella grandezza gia ce l avevo
            return ::recvfrom(_sockfd, &buf[0], buf.size(), flags, &remote.c_addr(), &remote.len())
        }
        // alla python
        std::pair<buffer, sockaddress<F>> recvfrom(size_t n, int flags=0) const
        {
            buffer buf(n);
            sockaddresses<F> remote;
            int nbytes = ::recvfrom(_sockfd, &buf[0], buf.size(), flags, &remote.c_addr(), &remote.len())
            return std::make_pair(buffer(buf.begin(),buf.begin+nbytes,remote))

        }
        

        ~socket()//distruttore
        {
            if(_sockfd!=-1)
            {
                ::close(_sockfd);
            }
        }

   
        
    };




};
#endif
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace eosio;



class [[eosio::contract]] tictactoe: public eosio::contract 
{
    public:
        using eosio::contract::contract;

        tictactoe (name self, name first_receiver, datastream<const char*> ds):
            contract(self, first_receiver, ds), _surveillance(self, self.value)
            {}

        struct [[eosio::table]] surveillance_record
        {
            name subject;
            eosio::time_point timestamp;

            uint64_t primary_key() const { return subject.value;}
            EOSLIB_SERIALIZE(surveillance_record,(subject)(timestamp))
        };
        [[eosio::action]] void welcome(name host, name challenger)
        {
            require_auth(host);

            auto itr = _surveillance.find(host.value);
            if(itr != _surveillance.end())
            {
                _surveillance.modify(itr, get_self(),[&](auto& new_row)
                {
                    new_row.subject = host;
                    new_row.timestamp = current_time_point();
                });
            }
            else
            {
                _surveillance.emplace(get_self(),[&](auto& new_row)
                {
                    new_row.subject = host;
                    new_row.timestamp = current_time_point();
                });
            }
            

            print("Welcome, challengers ",host);
            print(" and ", challenger);
        }

    private:
        using surveillance_index = eosio::multi_index< 
        name("surveillance"),
        surveillance_record
        >;

        surveillance_index _surveillance;

};
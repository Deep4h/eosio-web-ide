#include "../include/ordercontrct.hpp"

// Generic eosio library, i.e. print, type, math, etc
using namespace eosio;

namespace tte {

void ordercontrct::addproduct(const std::string &title, const std::string &description, const std::string &url, const float &price)
{
    const name payer = "ordercontrct"_n;
    require_auth(payer);

    products existing_products(get_self(), payer.value);

    uint16_t id = existing_products.available_primary_key();
    product new_product{id, title, description, url, price};
    existing_products.emplace( payer, [&](auto &g) {g = new_product;} );
    eosio::action(
        permission_level{get_self(), "active"_n},
        get_self(),
        "addedproduct"_n,
        new_product)
    .send();
}

void ordercontrct::addorder( uint16_t                      userid,
                         const std::vector<uint8_t> &  items,
                         const std::string &           status )
{
    addorder_impl(userid, items, status, false);
}

void ordercontrct::updateorder(uint16_t id, uint16_t userid, const std::vector<uint8_t> &items, const std::string &status)
{
    require_auth(contract_account);

    orders existing_orders(get_self(), contract_account.value);

    // Check if order already exists
    auto itr = existing_orders.find(id);
    check(itr != existing_orders.end(), "Unable to find an order with specified ID");

    // proceed with order modification
    existing_orders.modify(itr, contract_account, [&](auto &g) {
         g.id = id;
         g.userid=userid;
         g.status=status;
         g.total=0.;
         g.items = items;
    });
}

uint16_t ordercontrct::addorder_impl( uint16_t                      userid,
                                  const std::vector<uint8_t> &  items,
                                  const std::string &           status,
                                  bool                          invoke_return_value )
   {
      require_auth(contract_account);

      orders existing_orders(get_self(), contract_account.value);

      // proceed with order creation
      uint16_t id=existing_orders.available_primary_key();
      order new_order{id, userid, items, status, 0.0};
      existing_orders.emplace(contract_account, [&](auto &g) { g = new_order; });
      return id;
   }
}
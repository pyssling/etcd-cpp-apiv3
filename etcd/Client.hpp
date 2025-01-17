#ifndef __ETCD_CLIENT_HPP__
#define __ETCD_CLIENT_HPP__

#include <map>
#include <mutex>
#include <string>

#include <grpc++/grpc++.h>
#include "pplx/pplxtasks.h"

#include "etcd/Response.hpp"

namespace etcdv3 {
  class Transaction;
}

namespace etcd
{
  class KeepAlive;
  class Watcher;

  /**
   * Client is responsible for maintaining a connection towards an etcd server.
   * Etcd operations can be reached via the methods of the client.
   */
  class Client
  {
  public:
    /**
     * Constructs an etcd client object.
     *
     * @param etcd_url is the url of the etcd server to connect to, like "http://127.0.0.1:2379",
     *                 or multiple url, seperated by ',' or ';'.
     * @param load_balancer is the load balance strategy, can be one of round_robin/pick_first/grpclb/xds.
     */
    Client(std::string const & etcd_url,
           std::string const & load_balancer = "round_robin");

    /**
     * Constructs an etcd client object.
     *
     * @param etcd_url is the url of the etcd server to connect to, like "http://127.0.0.1:2379",
     *                 or multiple url, seperated by ',' or ';'.
     * @param username username of etcd auth
     * @param password password of etcd auth
     * @param load_balancer is the load balance strategy, can be one of round_robin/pick_first/grpclb/xds.
     */
    Client(std::string const & etcd_url,
           std::string const & username,
           std::string const & password,
           std::string const & load_balancer = "round_robin");

    /**
     * Constructs an etcd client object.
     *
     * @param etcd_url is the url of the etcd server to connect to, like "http://127.0.0.1:2379",
     *                 or multiple url, seperated by ',' or ';'.
     * @param username username of etcd auth
     * @param password password of etcd auth
     * @param load_balancer is the load balance strategy, can be one of round_robin/pick_first/grpclb/xds.
     */
    static etcd::Client *WithUser(std::string const & etcd_url,
           std::string const & username,
           std::string const & password,
           std::string const & load_balancer = "round_robin");

    /**
     * Constructs an etcd client object.
     *
     * @param etcd_url is the url of the etcd server to connect to, like "http://127.0.0.1:2379",
     *                 or multiple url, seperated by ',' or ';'.
     * @param ca   root CA file for SSL/TLS connection.
     * @param cert cert chain file for SSL/TLS authentication, could be empty string.
     * @param key  private key file for SSL/TLS authentication, could be empty string.
     * @param load_balancer is the load balance strategy, can be one of round_robin/pick_first/grpclb/xds.
     */
    Client(std::string const & etcd_url,
           std::string const & ca,
           std::string const & cert,
           std::string const & key,
           std::string const & load_balancer);

    /**
     * Constructs an etcd client object.
     *
     * @param etcd_url is the url of the etcd server to connect to, like "http://127.0.0.1:2379",
     *                 or multiple url, seperated by ',' or ';'.
     * @param ca   root CA file for SSL/TLS connection.
     * @param cert cert chain file for SSL/TLS authentication, could be empty string.
     * @param key  private key file for SSL/TLS authentication, could be empty string.
     * @param load_balancer is the load balance strategy, can be one of round_robin/pick_first/grpclb/xds.
     */
    static etcd::Client *WithSSL(std::string const & etcd_url,
           std::string const & ca,
           std::string const & cert = "",
           std::string const & key = "",
           std::string const & load_balancer = "round_robin");

    /**
     * Sends a get request to the etcd server
     * @param key is the key to be read
     */
    pplx::task<Response> get(std::string const & key);

    /**
     * Sets the value of a key. The key will be modified if already exists or created
     * if it does not exists.
     * @param key is the key to be created or modified
     * @param value is the new value to be set
     */
    pplx::task<Response> set(std::string const & key, std::string const & value, int ttl = 0);

    /**
     * Sets the value of a key. The key will be modified if already exists or created
     * if it does not exists.
     * @param key is the key to be created or modified
     * @param value is the new value to be set
     * @param leaseId is the lease attached to the key
     */
    pplx::task<Response> set(std::string const & key, std::string const & value, int64_t leaseId);


    /**
     * Creates a new key and sets it's value. Fails if the key already exists.
     * @param key is the key to be created
     * @param value is the value to be set
     */
    pplx::task<Response> add(std::string const & key, std::string const & value, int ttl = 0);

    /**
     * Creates a new key and sets it's value. Fails if the key already exists.
     * @param key is the key to be created
     * @param value is the value to be set
     * @param leaseId is the lease attached to the key
     */
    pplx::task<Response> add(std::string const & key, std::string const & value, int64_t leaseId);

    /**
     * Modifies an existing key. Fails if the key does not exists.
     * @param key is the key to be modified
     * @param value is the new value to be set
     */
    pplx::task<Response> modify(std::string const & key, std::string const & value, int ttl = 0);

    /**
     * Modifies an existing key. Fails if the key does not exists.
     * @param key is the key to be modified
     * @param value is the new value to be set
     * @param leaseId is the lease attached to the key
     */
    pplx::task<Response> modify(std::string const & key, std::string const & value, int64_t leaseId);

    /**
     * Modifies an existing key only if it has a specific value. Fails if the key does not exists
     * or the original value differs from the expected one.
     * @param key is the key to be modified
     * @param value is the new value to be set
     * @param old_value is the value to be replaced
     */
    pplx::task<Response> modify_if(std::string const & key, std::string const & value, std::string const & old_value, int ttl = 0);

    /**
     * Modifies an existing key only if it has a specific value. Fails if the key does not exists
     * or the original value differs from the expected one.
     * @param key is the key to be modified
     * @param value is the new value to be set
     * @param old_value is the value to be replaced
     * @param leaseId is the lease attached to the key
     */
    pplx::task<Response> modify_if(std::string const & key, std::string const & value, std::string const & old_value, int64_t leaseId);

    /**
     * Modifies an existing key only if it has a specific modification index value. Fails if the key
     * does not exists or the modification index of the previous value differs from the expected one.
     * @param key is the key to be modified
     * @param value is the new value to be set
     * @param old_index is the expected index of the original value
     */
    pplx::task<Response> modify_if(std::string const & key, std::string const & value, int old_index, int ttl = 0);

    /**
     * Modifies an existing key only if it has a specific modification index value. Fails if the key
     * does not exists or the modification index of the previous value differs from the expected one.
     * @param key is the key to be modified
     * @param value is the new value to be set
     * @param old_index is the expected index of the original value
     * @param leaseId is the lease attached to the key
     */
    pplx::task<Response> modify_if(std::string const & key, std::string const & value, int old_index, int64_t leaseId);

    /**
     * Removes a single key. The key has to point to a plain, non directory entry.
     * @param key is the key to be deleted
     */
    pplx::task<Response> rm(std::string const & key);

    /**
     * Removes a single key but only if it has a specific value. Fails if the key does not exists
     * or the its value differs from the expected one.
     * @param key is the key to be deleted
     */
    pplx::task<Response> rm_if(std::string const & key, std::string const & old_value);

    /**
     * Removes an existing key only if it has a specific modification index value. Fails if the key
     * does not exists or the modification index of it differs from the expected one.
     * @param key is the key to be deleted
     * @param old_index is the expected index of the existing value
     */
    pplx::task<Response> rm_if(std::string const & key, int old_index);

    /**
     * Gets a directory listing of the directory identified by the key.
     * @param key is the key to be listed
     */
    pplx::task<Response> ls(std::string const & key);


    /**
     * Gets a directory listing of the directory identified by the key.
     * @param key is the key to be listed
     * @param limit is the size limit of results to be listed, we don't use default parameters
     *        to ensure backwards binary compatibility.
     */
    pplx::task<Response> ls(std::string const & key, size_t const limit);

    /**
     * Gets a directory listing of the directory identified by the key and range_end, i.e., get
     * all keys in the range [key, range_end).
     *
     * @param key is the key to be listed
     * @param range_end is the end of key range to be listed
     */
    pplx::task<Response> ls(std::string const & key, std::string const &range_end);


    /**
     * Gets a directory listing of the directory identified by the key and range_end, i.e., get
     * all keys in the range [key, range_end).
     *
     * @param key is the key to be listed
     * @param range_end is the end of key range to be listed
     * @param limit is the size limit of results to be listed, we don't use default parameters
     *        to ensure backwards binary compatibility.
     */
    pplx::task<Response> ls(std::string const & key, std::string const &range_end, size_t const limit);

    /**
     * Removes a directory node. Fails if the parent directory dos not exists or not a directory.
     * @param key is the directory to be created to be listed
     * @param recursive if true then delete a whole subtree, otherwise deletes only an empty directory.
     */
    pplx::task<Response> rmdir(std::string const & key, bool recursive = false);

    /**
     * Removes multiple keys between [key, range_end).
     *
     * This overload for `const char *` is to avoid const char * to bool implicit casting.
     *
     * @param key is the directory to be created to be listed
     * @param range_end is the end of key range to be removed.
     */
    pplx::task<Response> rmdir(std::string const & key, const char *range_end);

    /**
     * Removes multiple keys between [key, range_end).
     *
     * @param key is the directory to be created to be listed
     * @param range_end is the end of key range to be removed.
     */
    pplx::task<Response> rmdir(std::string const & key, std::string const &range_end);

    /**
     * Watches for changes of a key or a subtree. Please note that if you watch e.g. "/testdir" and
     * a new key is created, like "/testdir/newkey" then no change happened in the value of
     * "/testdir" so your watch will not detect this. If you want to detect addition and deletion of
     * directory entries then you have to do a recursive watch.
     * @param key is the value or directory to be watched
     * @param recursive if true watch a whole subtree
     */
    pplx::task<Response> watch(std::string const & key, bool recursive = false);

    /**
     * Watches for changes of a key or a subtree from a specific index. The index value can be in the "past".
     * @param key is the value or directory to be watched
     * @param fromIndex the first index we are interested in
     * @param recursive if true watch a whole subtree
     */
    pplx::task<Response> watch(std::string const & key, int fromIndex, bool recursive = false);

    /**
     * Watches for changes of a range of keys inside [key, range_end).
     *
     * This overload for `const char *` is to avoid const char * to bool implicit casting.
     *
     * @param key is the value or directory to be watched
     * @param range_end is the end of key range to be removed.
     */
    pplx::task<Response> watch(std::string const & key, const char *range_end);

    /**
     * Watches for changes of a range of keys inside [key, range_end).
     *
     * @param key is the value or directory to be watched
     * @param range_end is the end of key range to be removed.
     */
    pplx::task<Response> watch(std::string const & key, std::string const &range_end);

    /**
     * Watches for changes of a range of keys inside [key, range_end) from a specific index. The index value
     * can be in the "past".
     *
     * Watches for changes of a key or a subtree from a specific index. The index value can be in the "past".
     * @param key is the value or directory to be watched
     * @param range_end is the end of key range to be removed.
     * @param fromIndex the first index we are interested in
     */
    pplx::task<Response> watch(std::string const & key, std::string const &range_end, int fromIndex);

    /**
     * Grants a lease.
     * @param ttl is the time to live of the lease
     */
    pplx::task<Response> leasegrant(int ttl);

    /**
     * Revoke a lease.
     * @param lease_id is the id the lease
     */
    pplx::task<Response> leaserevoke(int64_t lease_id);

    /**
     * Get time-to-live of a lease.
     * @param lease_id is the id the lease
     */
    pplx::task<Response> leasetimetolive(int64_t lease_id);

    /**
     * Gains a lock at a key, using a default created lease, using the default lease (10 seconds), with
     * keeping alive has already been taken care of by the library.
     * @param key is the key to be used to request the lock.
     */
    pplx::task<Response> lock(std::string const &key);

    /**
     * Gains a lock at a key, using a default created lease, using the specified lease TTL (in seconds), with
     * keeping alive has already been taken care of by the library.
     * @param key is the key to be used to request the lock.
     * @param lease_ttl is the TTL used to create a lease for the key.
     */
    pplx::task<Response> lock(std::string const &key, int lease_ttl);

    /**
     * Gains a lock at a key, using a user-provided lease, the lifetime of the lease won't be taken care
     * of by the library.
     * @param key is the key to be used to request the lock.
     */
    pplx::task<Response> lock(std::string const &key, int64_t lease_id);

    /**
     * Releases a lock at a key.
     * @param key is the lock key to release.
     */
    pplx::task<Response> unlock(std::string const &lock_key);

     /**
      * Execute a etcd transaction.
      * @param txn is the transaction object to be executed.
      */
    pplx::task<Response> txn(etcdv3::Transaction const &txn);

  private:
    std::shared_ptr<grpc::Channel> channel;
    std::string auth_token;

    struct EtcdServerStubs;
    struct EtcdServerStubsDeleter {
      void operator()(EtcdServerStubs *stubs);
    };
    std::unique_ptr<EtcdServerStubs, EtcdServerStubsDeleter> stubs;

    std::mutex mutex_for_keepalives;
    std::map<std::string, int64_t> leases_for_locks;
    std::map<int64_t, std::shared_ptr<KeepAlive>> keep_alive_for_locks;

    friend class KeepAlive;
    friend class Watcher;
};



}

#endif

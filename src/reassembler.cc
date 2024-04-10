#include "reassembler.hh"
#include <algorithm>
#include <ranges>
using namespace std;

auto Reassembler::split( uint64_t pos ) noexcept 
{
  // https://en.cppreference.com/w/cpp/container/set/lower_bound
  auto it { buf_.lower_bound( pos ) };
  if ( it != buf_.end() && it->first == pos){
      return it;
  }
  if ( it == buf_.begin() ) return it;
  if ( const auto pit { prev(it) }; pit->first + size( pit->second) > pos) {
    const auto res = buf_.emplace_hint(it, pos, pit->second.substr(pos - pit->first));
    pit->second.resize( pos - pit->first);
    return res;
  }
  return it;
};

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  const auto try_close = [&]() noexcept -> void {
    if( end_index_.has_value() && end_index_.value() == writer().bytes_pushed() ) {
      output_.writer().close();
    }
  };

  if( data.empty() ) { // No capacity limit
    if ( !end_index_.has_value() && is_last_substring) {
      end_index_.emplace( first_index );
    }
    return try_close();
  }

  if( writer().is_closed() || writer().available_capacity() == 0U ) {
    return;
  }

  // Reassembler's internal storage: [unassembled_index, unacceptable_index)
  const uint64_t unassembled_index { writer().bytes_pushed() };
  const uint64_t unacceptable_index { unassembled_index + writer().available_capacity() };
  if ( first_index + size( data ) <= unassembled_index || first_index >= unacceptable_index ) {
    return;
  }
  if ( first_index + size( data) > unacceptable_index ) { // Remove unacceptable bytes
    data.resize( unacceptable_index - first_index);
    is_last_substring = false;
  }
  // Remove poped/buffered bytes
  if ( first_index < unassembled_index ) {
    data.erase( 0, unassembled_index - first_index );
    first_index = unassembled_index;
  }

  if ( !end_index_.has_value() && is_last_substring ) {
    end_index_.emplace( first_index + size( data ) );
  }

  const auto upper { split( first_index + size( data ) ) };
  const auto lower { split( first_index ) };
  ranges::for_each( ranges::subrange( lower, upper ) | views::values, 
                    [&]( const auto& str ) { total_pending_ -= str.size(); } );
  total_pending_ += size ( data );
  // In summary, this code snippet first erases a range of elements from lower to upper (exclusive) from the buf_ map. 
  // Then, it inserts a new element with a key of first_index and a value of data into the map at the position indicated 
  // by the iterator returned by erase(). This combination of erase() and emplace_hint() allows for efficient removal and 
  // insertion of elements in a std::map
  buf_.emplace_hint( buf_.erase( lower, upper), first_index, move ( data ) );
  while ( !buf_.empty() ) {
    auto&& [index, payload] { *buf_.begin() };
    if ( index != writer().bytes_pushed() ) {
      break;
    }
    total_pending_ -= size( payload );
    output_.writer().push( move ( payload ));
    buf_.erase( buf_.begin() );
  }                  
  return try_close();
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return total_pending_;
}

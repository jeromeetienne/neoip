# to get the random kad_id
import random
# import hash function to derive kad_id from string
import sha512	# this one is a very custom one
				# - gotten from shax-py which isnt standard
				# - python 2.5 has a haslib which is standard but currently i got only 2.4
				# - to backport when hashlib is backported on 2.4
import sha


class kad_id:
	'''Class to contain a kad_id'''
	
	def __init__( self, val = long( 0 ) ):
		'''Constructor by default'''
		if isinstance( val, kad_id ):
			# if the parameter is another kad_id
			self.val	= val.val
		elif isinstance( val, int ) or isinstance( val, long ):
			# if the parameter is an int or a long
			self.val	= long( val )
		elif isinstance( val, str ) and val[:2] == "0x":
			# if the parameter is an string representing a hexadecimal number
			self.val	= long( val, 16 )
		elif isinstance( val, str ):
			# if the parameter is an string to derive thru a hash
			self.val	= kad_id.derive_from_str(val)
		else:
			assert 0

	def dist( self, other_id ):
		'''Compute the distance between the two id based on the XOR'''
		return kad_id( self.val ^ other_id.val )

	def is_null( self ):
		'''Return true is the kad_id is null'''
		return self == kad_id()

	def rand():
		'''Return a random kad_id'''
		return kad_id( random.randint( 0, 2**kad_id.get_nbit() ) )
	rand = staticmethod( rand )

	def get_nbit():
		'''return the number of bit used by a kad_id'''
		return 20*8
	get_nbit	= staticmethod( get_nbit )

	def derive_from_str(value):
		'''Return a kad_id derived from a string'''
		# sanity check - the value MUST be a string
		assert isinstance(value, str)
		# if the size of the kad_id is greater than 160, use sha512
		if kad_id.get_nbit() > 160:
			_hash = sha512.new()
		else:
			_hash = sha.new()
		# compute the hash digest of the value
		_hash.update(value)		
		return int(_hash.hexdigest()[:kad_id.get_nbit()/4], 16)

	derive_from_str = staticmethod( derive_from_str )

	def to_xmlrpc( self ):
		'''Convert the object to be sent thru a xmlrpc'''
		return str( self )

	def from_xmlrpc( str ):
		'''Convert the object just received from a xmlrpc'''
		return kad_id( str )
	from_xmlrpc	= staticmethod( from_xmlrpc )

	def __str__( self ):
		'''Compute the INformal string representation'''
		return '0x' + str( '%x' % self.val ).rjust( kad_id.get_nbit()/4, '0' )

	def __repr__( self ):
		'''Compute the formal string representation'''
		return str( self )

	def __cmp__( self, other ):
		'''Comparison operator for kad_id'''
		if self.val < other.val:
			return -1
		elif self.val > other.val:
			return +1
		else:
			return 0

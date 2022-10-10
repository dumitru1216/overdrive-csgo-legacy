#include "math.h"
#include "../hack/sdk/src_kit.h"

namespace math {

	void normalize_angles( Vector& angles ) {
		while ( angles.y <= -180 ) angles.y += 360;
		while ( angles.y > 180 ) angles.y -= 360;
		while ( angles.x <= -180 ) angles.x += 360;
		while ( angles.x > 180 ) angles.x -= 360;


		if ( angles.x > 89.0 ) angles.x = 89;
		if ( angles.x < -89.0 ) angles.x = -89;
		if ( angles.y < -180 ) angles.y = -179.999;
		if ( angles.y > 180 ) angles.y = 179.999;

		angles.z = 0;
	}

	float approach_angle( float fl_target, float fl_value, float fl_speed )
	{
		auto fl_angle_mod = [ ]( float fl_angle ) -> float
		{
			return ( ( 360.0f / 65536.0f ) * ( ( int32_t )( fl_angle * ( 65536.0f / 360.0f ) ) & 65535 ) );
		};

		float fl_adjusted_speed = fl_speed;

		if ( fl_adjusted_speed < 0.0f )
			fl_adjusted_speed *= -1.0f;

		float fl_angle_mod_target = fl_angle_mod( fl_target );
		float fl_angle_mod_value = fl_angle_mod( fl_value );

		float fl_delta = ( fl_angle_mod_target - fl_angle_mod_value );

		if ( fl_delta >= -180.0f )
		{
			if ( fl_delta >= 180.0f )
				fl_delta -= 360.0f;
		}
		else
		{
			if ( fl_delta <= -180.0f )
				fl_delta += 360.0f;
		}

		float fl_return;

		if ( fl_delta <= fl_adjusted_speed )
		{
			if ( ( fl_adjusted_speed * -1.0f ) <= fl_delta )
				fl_return = fl_angle_mod_target;
			else
				fl_return = ( fl_angle_mod_value - fl_adjusted_speed );
		}
		else
			fl_return = ( fl_angle_mod_value + fl_adjusted_speed );

		return fl_return;
	}

	float angle_distance( float firstangle, float secondangle )
	{
		if ( firstangle == secondangle )
			return 0.f;

		bool oppositeSides = false;

		if ( firstangle > 0 && secondangle < 0 )
			oppositeSides = true;
		else if ( firstangle < 0 && secondangle > 0 )
			oppositeSides = true;

		if ( !oppositeSides )
			return fabs( firstangle - secondangle );

		bool past90 = false;

		if ( firstangle > 90 && secondangle < -90 )
		{
			firstangle -= ( firstangle - 90 );
			secondangle += ( secondangle + 90 );
		}
		else if ( firstangle < -90 && secondangle > 90 )
		{
			firstangle += ( firstangle + 90 );
			secondangle -= ( secondangle - 90 );
		}

		float oneTwo;

		oneTwo = fabs( firstangle - secondangle );

		return oneTwo;
	}

	float lowerbody_rotate( float lby, float yaw ) {
		float delta = math::clamp_yaw || math::normalize_angle || math::normalize_yaw( yaw - lby );
		if ( fabs( delta ) < 25.f )
			return lby;

		if ( delta > 0.f )
			return yaw + 25.f;

		return yaw;
	}

	void normalize_angle( QAngle& angles ) {
		for ( auto i = 0; i < 3; i++ ) {
			while ( angles[ i ] < -180.0f ) angles[ i ] += 360.0f;
			while ( angles[ i ] > 180.0f ) angles[ i ] -= 360.0f;
		}
	}

	void normalize_vector( Vector& vector ) {
		for ( auto i = 0; i < 3; i++ ) {
			while ( vector[ i ] < -180.0f ) vector[ i ] += 360.0f;
			while ( vector[ i ] > 180.0f ) vector[ i ] -= 360.0f;
		}

		vector[ 2 ] = 0.f;
	}

	//--------------------------------------------------------------------------------
	float normalize_yaw( float f ) {
		if ( std::isnan( f ) || std::isinf( f ) )
			f = 0;

		if ( f > 9999999 )
			f = 0;

		if ( f < -9999999 )
			f = 0;

		while ( f < -180.0f ) f += 360.0f;
		while ( f > 180.0f ) f -= 360.0f;

		return f;
	}

	float normalize_pitch( float pitch ) {
		while ( pitch > 89.f )
			pitch -= 180.f;

		while ( pitch < -89.f )
			pitch += 180.f;

		return pitch;
	}
	//--------------------------------------------------------------------------------
	float vector_normalize( Vector& v ) {
		float l = v.Length( );

		if ( l != 0.0f )
			v /= l;
		else
			v.x = v.y = 0.0f; v.z = 1.0f;

		return l;
	}
	//--------------------------------------------------------------------------------
	void clamp_angles( Vector& angles ) {
		if ( angles.x > 89.0f ) angles.x = 89.0f;
		else if ( angles.x < -89.0f ) angles.x = -89.0f;

		if ( angles.y > 180.0f ) angles.y = 180.0f;
		else if ( angles.y < -180.0f ) angles.y = -180.0f;

		angles.z = 0;
	}
	//--------------------------------------------------------------------------------
	void vector_transform( const Vector& in1, const matrix3x4_t& in2, Vector& out ) {
		auto DotProduct = [ ]( const Vector& a, const Vector& b ) {
			return ( a.x * b.x + a.y * b.y + a.z * b.z );
		};

		out[ 0 ] = DotProduct( in1, Vector( in2[ 0 ][ 0 ], in2[ 0 ][ 1 ], in2[ 0 ][ 2 ] ) ) + in2[ 0 ][ 3 ];
		out[ 1 ] = DotProduct( in1, Vector( in2[ 1 ][ 0 ], in2[ 1 ][ 1 ], in2[ 1 ][ 2 ] ) ) + in2[ 1 ][ 3 ];
		out[ 2 ] = DotProduct( in1, Vector( in2[ 2 ][ 0 ], in2[ 2 ][ 1 ], in2[ 2 ][ 2 ] ) ) + in2[ 2 ][ 3 ];
	}
	//--------------------------------------------------------------------------------
	void angle_vectors( const Vector& angles, Vector& forward ) {
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos( &sp, &cp, DEG2RAD( angles[ 0 ] ) );
		DirectX::XMScalarSinCos( &sy, &cy, DEG2RAD( angles[ 1 ] ) );

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	void angle_vectors( const QAngle& angles, Vector& forward ) {
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos( &sp, &cp, DEG2RAD( angles[ 0 ] ) );
		DirectX::XMScalarSinCos( &sy, &cy, DEG2RAD( angles[ 1 ] ) );

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	Vector angle_vectors( const Vector& angle ) {
		float sp, sy, cp, cy;

		DirectX::XMScalarSinCos( &sp, &cp, DEG2RAD( angle[ 0 ] ) );
		DirectX::XMScalarSinCos( &sy, &cy, DEG2RAD( angle[ 1 ] ) );

		Vector forward;
		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;

		return forward;
	}
	//--------------------------------------------------------------------------------
	Vector calculate_angle( Vector src, Vector dst ) {
		Vector angles;

		Vector delta = src - dst;
		float hyp = delta.Length2D( );

		angles.y = std::atanf( delta.y / delta.x ) * 57.2957795131f;
		angles.x = std::atanf( -delta.z / hyp ) * -57.2957795131f;
		angles.z = 0.0f;

		if ( delta.x >= 0.0f )
			angles.y += 180.0f;

		return angles;
	}
	//--------------------------------------------------------------------------------
	float get_fov( Vector view_angle, Vector aim_angle ) {
		const float MaxDegrees = 360.0f;
		Vector Delta( 0, 0, 0 );
		Vector Forward( 0, 0, 0 );
		Vector delta = aim_angle - view_angle;
		clamp_angles( delta );
		return sqrtf( powf( delta.x, 2.0f ) + powf( delta.y, 2.0f ) );
	}
	//--------------------------------------------------------------------------------
	void angle_vectors( const Vector& angles, Vector* forward, Vector* right, Vector* up ) {
		auto sincos = [ ]( float radians, float* sine, float* cosine ) {
			*sine = sin( radians );
			*cosine = cos( radians );
		};

		float sr, sp, sy, cr, cp, cy;

		sincos( DEG2RAD( angles[ 1 ] ), &sy, &cy );
		sincos( DEG2RAD( angles[ 0 ] ), &sp, &cp );
		sincos( DEG2RAD( angles[ 2 ] ), &sr, &cr );

		if ( forward ) {
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if ( right ) {
			right->x = ( -1 * sr * sp * cy + -1 * cr * -sy );
			right->y = ( -1 * sr * sp * sy + -1 * cr * cy );
			right->z = -1 * sr * cp;
		}

		if ( up ) {
			up->x = ( cr * sp * cy + -sr * -sy );
			up->y = ( cr * sp * sy + -sr * cy );
			up->z = cr * cp;
		}
	}
	//--------------------------------------------------------------------------------
	void vector_angles( const Vector& forward, Vector& angles ) {
		float	tmp, yaw, pitch;

		if ( forward[ 1 ] == 0 && forward[ 0 ] == 0 ) {
			yaw = 0;
			if ( forward[ 2 ] > 0 )
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = ( atan2( forward[ 1 ], forward[ 0 ] ) * 180 / DirectX::XM_PI );
			if ( yaw < 0 )
				yaw += 360;

			tmp = sqrt( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );
			pitch = ( atan2( -forward[ 2 ], tmp ) * 180 / DirectX::XM_PI );
			if ( pitch < 0 )
				pitch += 360;
		}

		if ( forward.x >= 0.0f ) yaw += 180;

		angles[ 0 ] = pitch;
		angles[ 1 ] = yaw;
		angles[ 2 ] = 0;
	}
	//--------------------------------------------------------------------------------
	void vector_angles( const Vector& forward, QAngle& angles )
	{
		float tmp, yaw, pitch;

		if ( forward[ 1 ] == 0 && forward[ 0 ] == 0 ) {
			yaw = 0;
			if ( forward[ 2 ] > 0 )
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = ( atan2( forward[ 1 ], forward[ 0 ] ) * 180 / DirectX::XM_PI );
			if ( yaw < 0 )
				yaw += 360;

			tmp = sqrt( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );
			pitch = ( atan2( -forward[ 2 ], tmp ) * 180 / DirectX::XM_PI );
			if ( pitch < 0 )
				pitch += 360;
		}

		if ( forward.x >= 0.0f ) yaw += 180;

		angles[ 0 ] = pitch;
		angles[ 1 ] = yaw;
		angles[ 2 ] = 0;
	}
	//--------------------------------------------------------------------------------
	void vector_angles( const Vector& forward, Vector& up, Vector& angles ) {
		Vector left = cross_product( up, forward );
		left.NormalizeInPlace( );

		float forward_dist = forward.Length2D( );

		if ( forward_dist > 0.001f ) {
			angles.x = atan2f( -forward.z, forward_dist ) * 180 / DirectX::XM_PI;
			angles.y = atan2f( forward.y, forward.x ) * 180 / DirectX::XM_PI;

			float upZ = ( left.y * forward.x ) - ( left.x * forward.y );
			angles.z = atan2f( left.z, upZ ) * 180 / DirectX::XM_PI;
		}
		else {
			angles.x = atan2f( -forward.z, forward_dist ) * 180 / DirectX::XM_PI;
			angles.y = atan2f( -left.x, left.y ) * 180 / DirectX::XM_PI;
			angles.z = 0;
		}
	}

	//--------------------------------------------------------------------------------
	Vector cross_product( const Vector& a, const Vector& b ) {
		Vector result;
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;

		return  result;
	}
	//--------------------------------------------------------------------------------
	float dot_product( const Vector& a, const Vector& b ) {
		return ( a.x * b.x
			+ a.y * b.y
			+ a.z * b.z );
	}
	//--------------------------------------------------------------------------------
	float random_float( float min, float max ) {
		typedef float( *RandomFloat_t )( float, float );
		static RandomFloat_t m_RandomFloat = ( RandomFloat_t )GetProcAddress( GetModuleHandle( "vstdlib.dll" ), "RandomFloat" );
		return m_RandomFloat( min, max );
	}
	//--------------------------------------------------------------------------------
	void random_seed( int seed ) {
		typedef void( *RandomSeed_t )( int );
		static RandomSeed_t m_RandomSeed = ( RandomSeed_t )GetProcAddress( GetModuleHandle( "vstdlib.dll" ), "RandomSeed" );
		return m_RandomSeed( seed );
	}
	//--------------------------------------------------------------------------------
	Vector vector_angles( const Vector& forward ) {
		Vector angles;
		float tmp, yaw, pitch;

		if ( forward[ 1 ] == 0 && forward[ 0 ] == 0 ) {
			yaw = 0;
			if ( forward[ 2 ] > 0 )
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = ( atan2( forward[ 1 ], forward[ 0 ] ) * 180 / DirectX::XM_PI );

			if ( yaw < 0 )
				yaw += 360;

			tmp = sqrt( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );
			pitch = ( atan2( -forward[ 2 ], tmp ) * 180 / DirectX::XM_PI );

			if ( pitch < 0 )
				pitch += 360;
		}

		angles[ 0 ] = pitch;
		angles[ 1 ] = yaw;
		angles[ 2 ] = 0;
		return angles;
	}
	//--------------------------------------------------------------------------------
	float distance_to_ray( const Vector& pos, const Vector& ray_start, const Vector& ray_end, float* along, Vector* point_on_ray )
	{
		Vector to = pos - ray_start;
		Vector dir = ray_end - ray_start;
		float length = dir.Normalized( ).Length( );

		float range_along = dir.Dot( to );
		if ( along )
			*along = range_along;

		float range;

		if ( range_along < 0.0f )
		{
			range = -( pos - ray_start ).Length( );

			if ( point_on_ray )
				*point_on_ray = ray_start;
		}
		else if ( range_along > length )
		{
			range = -( pos - ray_end ).Length( );

			if ( point_on_ray )
				*point_on_ray = ray_end;
		}
		else
		{
			Vector on_ray = ray_start + range_along * dir;
			range = ( pos - on_ray ).Length( );

			if ( point_on_ray )
				*point_on_ray = on_ray;
		}

		return range;
	}
	//--------------------------------------------------------------------------------
	Vector get_transformation( const Vector& in, const matrix3x4_t& matrix ) {
		Vector out;

		for ( int i = 0; i < 3; i++ )
			out[ i ] = in.Dot( Vector( matrix[ i ][ 0 ], matrix[ i ][ 1 ], matrix[ i ][ 2 ] ) ) + matrix[ i ][ 3 ];

		return out;
	}
	//--------------------------------------------------------------------------------
	void normalize_in_out( Vector& vIn, Vector& vOut )
	{
		float flLen = vIn.Length( );
		if ( flLen == 0 ) {
			vOut.Init( 0, 0, 1 );
			return;
		}
		flLen = 1 / flLen;
		vOut.Init( vIn.x * flLen, vIn.y * flLen, vIn.z * flLen );
	}
	//--------------------------------------------------------------------------------
	void vec_transform( const Vector& in1, const matrix3x4_t& in2, Vector& out ) {
		out.x = in1.x * in2[ 0 ][ 0 ] + in1.y * in2[ 0 ][ 1 ] + in1.z * in2[ 0 ][ 2 ] + in2[ 0 ][ 3 ];
		out.y = in1.x * in2[ 1 ][ 0 ] + in1.y * in2[ 1 ][ 1 ] + in1.z * in2[ 1 ][ 2 ] + in2[ 1 ][ 3 ];
		out.z = in1.x * in2[ 2 ][ 0 ] + in1.y * in2[ 2 ][ 1 ] + in1.z * in2[ 2 ][ 2 ] + in2[ 2 ][ 3 ];
	}
	//--------------------------------------------------------------------------------
	void angle_vectors( const Vector angles, Vector& forward, Vector& right, Vector& up )
	{
		float angle;
		static float sp, sy, cp, cy;

		angle = angles[ 0 ] * ( M_PI / 180.f );
		sp = sin( angle );
		cp = cos( angle );

		angle = angles[ 1 ] * ( M_PI / 180.f );
		sy = sin( angle );
		cy = cos( angle );


		forward[ 0 ] = cp * cy;
		forward[ 1 ] = cp * sy;
		forward[ 2 ] = -sp;


		static float sr, cr;

		angle = angles[ 2 ] * ( M_PI / 180.f );
		sr = sin( angle );
		cr = cos( angle );


		right[ 0 ] = -1 * sr * sp * cy + -1 * cr * -sy;
		right[ 1 ] = -1 * sr * sp * sy + -1 * cr * cy;
		right[ 2 ] = -1 * sr * cp;

		up[ 0 ] = cr * sp * cy + -sr * -sy;
		up[ 1 ] = cr * sp * sy + -sr * cy;
		up[ 2 ] = cr * cp;
	}
	//--------------------------------------------------------------------------------
	Vector normalize( Vector angs )
	{
		while ( angs.y < -180.0f )
			angs.y += 360.0f;
		while ( angs.y > 180.0f )
			angs.y -= 360.0f;
		if ( angs.x > 89.0f )
			angs.x = 89.0f;
		if ( angs.x < -89.0f )
			angs.x = -89.0f;
		angs.z = 0;
		return angs;
	}
	//--------------------------------------------------------------------------------
	float segment_to_segment( const vec3_t s1, const vec3_t s2, const vec3_t k1, const vec3_t k2 )
	{
		static auto constexpr epsilon = 0.00000001;

		auto u = s2 - s1;
		auto v = k2 - k1;
		const auto w = s1 - k1;

		const auto a = u.Dot( u );
		const auto b = u.Dot( v );
		const auto c = v.Dot( v );
		const auto d = u.Dot( w );
		const auto e = v.Dot( w );
		const auto D = a * c - b * b;
		float sn, sd = D;
		float tn, td = D;

		if ( D < epsilon ) {
			sn = 0.0;
			sd = 1.0;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if ( sn < 0.0 ) {
				sn = 0.0;
				tn = e;
				td = c;
			}
			else if ( sn > sd ) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if ( tn < 0.0 ) {
			tn = 0.0;

			if ( -d < 0.0 )
				sn = 0.0;
			else if ( -d > a )
				sn = sd;
			else {
				sn = -d;
				sd = a;
			}
		}
		else if ( tn > td ) {
			tn = td;

			if ( -d + b < 0.0 )
				sn = 0;
			else if ( -d + b > a )
				sn = sd;
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const float sc = abs( sn ) < epsilon ? 0.0 : sn / sd;
		const float tc = abs( tn ) < epsilon ? 0.0 : tn / td;

		m128 n;
		auto dp = w + u * sc - v * tc;
		n.f[ 0 ] = dp.Dot( dp );
		const auto calc = sqrt_ps( n.v );
		return reinterpret_cast< const m128* >( &calc )->f[ 0 ];
	}
}
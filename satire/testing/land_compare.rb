#land.c  
# 
#=============================================================  
# 
#Copyright 1996-2024 Tom Barbalet. All rights reserved.  
# 
#Permission is hereby granted, free of charge, to any person  
#obtaining a copy of this software and associated documentation  
#files (the "Software"), to deal in the Software without  
#restriction, including without limitation the rights to use,  
#copy, modify, merge, publish, distribute, sublicense, and/or  
#sell copies of the Software, and to permit persons to whom the  
#Software is furnished to do so, subject to the following  
#conditions:  
# 
#The above copyright notice and this permission notice shall be  
#included in all copies or substantial portions of the Software.  
# 
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,  
#EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES  
#OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND  
#NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT  
#HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,  
#WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING  
#FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  
#OTHER DEALINGS IN THE SOFTWARE.  
# 
#This software is a continuing work of Tom Barbalet, begun on  
#13 June 1996. No apes or cats were harmed in the writing of  
#this software.  



$MAP_DIMENSION = 512  

$genetics = Array.new(2)

$topography = Array.new(2 * 512 * 512)

def math_random()      
	tmp0 = 0    
	tmp1 = 0    
	runIt = 1    

	tmp0 = $genetics[0] 
	tmp1 = $genetics[1] 

	$genetics[0] = tmp1 
	tempAnd7 = tmp0 & 7    

	if (tempAnd7 == 0)   
		$genetics[0] = ( tmp1 ^ ( tmp0 >> 3 ) ^ 23141 ) 
		runIt = 0 
	end  
	if (tempAnd7 == 3)   
		$genetics[1] = ( tmp0 ^ ( tmp1 >> 1 ) ^ 53289 ) 
		runIt = 0 
	end  
	if (tempAnd7 == 5)   
		$genetics[1] = ( tmp1 ^ ( tmp0 >> 2 ) ^ 44550 ) 
		runIt = 0 
	end  
	if (runIt == 1)   
		$genetics[1] = ( tmp0 ^ ( tmp1 >> 1 ) ) 
	end  
	return $genetics[1] 
end  

def tiles_non_planet( lx, ly )      
	converted_x = 0    
	converted_y = 0    
	converted_x = ( lx + $MAP_DIMENSION ) & ( $MAP_DIMENSION - 1 ) 
	converted_y = ( ly + $MAP_DIMENSION ) & ( $MAP_DIMENSION - 1 ) 
	return ( converted_x | ( converted_y * $MAP_DIMENSION ) ) 
end  

def tiles_topography( buffer, lx, ly )      
	return $topography[(buffer * 512 * 512) + tiles_non_planet( lx, ly )] 
end  

def tiles_set_topography( buffer, lx, ly, value )      
	$topography[(buffer * 512 * 512) + tiles_non_planet( lx, ly )] = value 
end  

def tiles_swap_topography()      
	loop = 0    
	while (loop < (512 * 512))   
		$topography[(512 * 512) + loop] = $topography[loop] 
		loop += 1 
	end  
end  

def title_pack_topography()      
	loop = 0    
	while (loop < (512 * 512))   
		$topography[loop] = 128 
		loop += 1 
	end  
end  

def tile_round()      
	local_tile_dimension = 1 << 9    
	span_minor = 0    
	while ( span_minor < 6 )   
		py = 0    
		while ( py < local_tile_dimension )   
			px = 0    
			while ( px < local_tile_dimension )   
				sum = 0    
				ty = -1    
				while ( ty < 2 )   
					tx = -1    
					while ( tx < 2 )   
						sum += tiles_topography( ( span_minor & 1 ), px + tx, py + ty ) 
						tx += 1 
					end  
					ty += 1 
				end  
				tiles_set_topography( ( span_minor & 1 ) ^ 1, px, py, sum / 9 ) 
				px += 1 
			end  
			py += 1 
		end  
		span_minor += 1 
	end  
end  

def tile_patch( refine )      
	local_tiles = 2    
	span_minor = 0    
	span_major = 0    
	tile_y = 0    
	span_minor = ( 64 >> ( ( refine & 7 ) ^ 7 ) ) 
	span_major = ( 1 << ( ( refine & 7 ) ^ 7 ) ) 
	while ( tile_y < local_tiles )   
		tile_x = 0    
		while ( tile_x < local_tiles )   
			py = 0    
			while ( py < span_minor )   
				px = 0    
				while ( px < span_minor )   
					val1 = 0    
					ty = 0    
					tseed = 0    
					tseed = math_random() 
					val1 = ( ( px << 2 ) + ( py << 10 ) ) 
					while ( ty < 4 )   
						tx = 0    
						while ( tx < 4 )   
							val2 = 0    
							val3 = 0    
							my = 0    

							val2 = ( tseed >> ( tx | ( ty << 2 ) ) ) 
							val3 = ( ( ( ( val2 & 1 ) << 1 ) - 1 ) * 20 ) 

							val2 = ( tx | ( ty << 8 ) ) 

							while ( my < span_major )   
								mx = 0    
								while ( mx < span_major )   
									local_map_po= 0    
									po= 0    
									pointx = 0    
									pointy = 0    
									
									point = ( ( mx | ( my << 8 ) ) + ( span_major * ( val1 + val2 ) ) ) 
									pointx = ( point & 255 ) 
									pointy = ( point >> 8 ) 
									if (( refine & 2 ) != 0)   
										pointx_tmp = pointx + pointy    
										pointy = pointx - pointy 
										pointx = pointx_tmp 
									end  
									local_map_point = tiles_topography( 0, pointx + ( tile_x << 8 ), pointy + ( tile_y << 8 ) ) + val3 

									if ( local_map_point < 0 )   
										local_map_point = 0 
									end  
									if ( local_map_point > 255 )   
										local_map_point = 255 
									end  

									tiles_set_topography( 0, pointx + ( tile_x << 8 ), pointy + ( tile_y << 8 ), local_map_point ) 
									mx += 1 
								end  
								my += 1 
							end  
							tx += 1 
						end  
						ty += 1 
					end  
					px += 1 
				end  
				py += 1 
			end  
			tile_x += 1 
		end  
		tile_y += 1 
	end  
end  

def land_seed_genetics( r1, r2 )      
	$genetics[0] = r1 
	$genetics[1] = r2 

	$genetics[0] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) ) 
	$genetics[1] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) ) 

	math_random() 
	math_random() 
	math_random() 

	$genetics[0] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) ) 
	$genetics[1] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) ) 
end  

def land_init()      
	refine = 0    
	title_pack_topography() 
	while ( refine < 7 )   
		tile_patch( refine ) 
		tile_round() 
		tiles_swap_topography() 
		refine += 1 
	end  
end  

#Comment out if not run with other code  

land_seed_genetics(5162,14583) 
puts "Land start"    
land_init() 
puts "Land end"    
puts "Expect 159"    
puts $topography[249614]      
puts "Expect 97"    
puts $topography[242701]      
puts "Expect 151"    
puts $topography[42701]      
puts "Expect 99"    
puts $topography[9614]      
  


function plarea = area3D(p)
% TODO: Falta actualizar la descripción

% AREA3D  Area of a 3D planar polygon which does not lie in the x-y plane.
%	AREA3D(X,Y,Z) calculates the area of a polygon in space
%	formed by vertices with coordinate vectors X,Y and Z.
%	If the coordinates of vertex v_i are x_i, y_i and z_i
%	twice the area of a polygon is given by:
%	2 A(P) = abs(N . (sum_{i=0}^{n-1} (v_i x v_{i+1})))
%	where N is a unit vector normal to the plane. The `.' represents the
%	dot product operator, the `x' represents the cross product operator,
%	and abs() is the absolute value function.	
%	
% Autor: Ioan M. Buciu (nelu@zeus.csd.auth.gr)
% Creado: 04/2000
% Última modificación: 08/2007 por Fabián Tommasini

% Comments: This code has a bug that will cause it to fail whenever the first
% two segments of the polygon in question lie parallel.
% For example, consider the simple triangle in the plane that has area 1/2.
% 
% X = [0 .5 1 1 0];
% Y = [0 0 0 1 0];
% Z = [0 0 0 0 0];
% 
% polyarea(X,Y)
% ans =
% 0.5
% 
% What happens with area3D? Try it out.
% 
% area3D(X,Y,Z)
% Warning: Divide by zero.
% > In area3D at 34
% ans =
% NaN
% 
% A better algorithm would use a rotation into the (x,y) plane followed by 
% a call to polyarea. This would be both faster and more robust.

vert = get(p, 'Vertices');
x = vert(:,1);
y = vert(:,2);
z = vert(:,3);

%	Length of vectors X,Y and Z
lx = length(x);
ly = length(y);
lz = length(z);

%	Auxilliars needed for normals length
edge0 = [x(2) - x(1),y(2) - y(1),z(2) - z(1)];
edge1 = [x(3) - x(1),y(3) - y(1),z(3) - z(1)]; 

%	Cross products
nor3 = [edge0(2)*edge1(3) - edge0(3)*edge1(2),...
      edge0(3)*edge1(1) - edge0(1)*edge1(3),...
      edge0(1)*edge1(2) - edge0(2)*edge1(1)];

%	Length of normal vectors
inveln = 1/(sqrt(nor3(1)*nor3(1) + nor3(2)*nor3(2) + nor3(3)*nor3(3)));

%	Make normals unit length
nor3 = inveln*nor3;

for	i = 1:lx-1
   	csumx(i) = y(i)*z(i+1) - z(i)*y(i+1);
   	csumy(i) = z(i)*x(i+1) - x(i)*z(i+1);
   	csumz(i) = x(i)*y(i+1) - y(i)*x(i+1);
end

csumx = sum([csumx y(ly)*z(1)-z(lz)*y(1)]);
csumy = sum([csumy z(lz)*x(1)-x(lx)*z(1)]);
csumz = sum([csumz x(lx)*y(1)-y(ly)*x(1)]);

%	Calculate area
plarea = (abs(nor3(1)*csumx + nor3(2)*csumy + nor3(3)*csumz))/2;
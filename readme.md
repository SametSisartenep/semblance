# semblance

Semblance wants to be a shading language.

The idea is for it to have a translator that turns semblance shaders
into C that can be used with [libgraphics][1], and later turn it into a
VM that runs in [renderfs][2].

It's largely influenced by the [RSL][3] and [GLSL][4] languages.

## Syntax

	- declare and use custom structs
	- new types
		vector	→ Point3 (x,y,z,0)
		point	→ Point3 (x,y,z,w)
		point2	→ Point2 (x,y,w)
		pointN	→ PointN (x,y,[w|z,w]) | N ∈ {2,3} ???
		normal	→ Point3 (x,y,z,0) | {x,y,z} ∈ [0,1]
		color	→ Color
		matrix	→ Matrix[23] ???
		quat	→ Quaternion
	- new operators
		Matrix3 m, n;
		Point3 a, b;
		double s;

		b = b * a;	/* b = modulatept3(b, a); */
		b = b * s;	/* b = mulpt3(b, s); */
		b = b * m;	/* b = xform3(b, m); */
		b = b × a;	/* b = crossvec3(b, a); */
		b = b · a;	/* b = dotvec3(b, a); */
		m = m * n;	/* mulm3(m, n); */

		Matrix3 m2:
		m2 = m * n;	/* identity3(m2); mulm3(m2, m); mulm3(m2, n); */
				/* alt.: memmove(m2, m, 4*4*sizeof(double)); mulm3(m2, n); */
	- function polymorphism
		Point3 a, b;
		double c, d;

		a = lerp(a, b, 0.4);	/* lerp3(2); */
		c = lerp(c, d, 0.1);	/* flerp(2); */

		Texture *tex;
		Cubemap *cm;
		Point3 dir;
		Point2 uv;
		Color c;

		c = sample(tex, uv, nearest);	/* sampletexture(2) */
		c = sample(cm, dir, bilinear);	/* samplecubemap(2) */

		int a, b;
		double c, d;
		Point3 p0, p1;

		a = min(a, b);
		c = min(c, d);
		p0 = min(p0, p1);	/* component-wise */
	- stage-dependent shader parameters
		- vertex
			|   in shader   | in libgraphics |
			+--------------------------------+
			position	→ sp->v->p
			normal		→ sp->v->n
			color		→ sp->v->c
			uv		→ sp->v->uv
			material	→ sp->v->mtl
			tangent		→ sp->v->tangent
			<attr>		→ sp->v->attrs["<attr>"]
		- fragment
			|   in shader   | in libgraphics |
			+--------------------------------+
			position	→ sp->v->p
			color		→ sp->v->c
			<attr>		→ sp->v->attrs["<attr>"]
	- swizzles
		Point3 p;
		Color c;

		c.rgba = p.xyzw;
		c.rgb = p.xyz;
		p.zyx = c.rgb;
	- builtins
		In the following cases the absent parameters are replaced by:

			Entity*		→ sp->su->entity
			Camera*		→ sp->su->camera
			Framebuf*	→ sp->su->fb

		when translated for libgraphics.

		Point3 model2world(Point3)	/* Point3 model2world(Entity*, Point3); */
		Point3 world2vcs(Point3)	/* Point3 world2vcs(Camera*, Point3); */
		Point3 vcs2clip(Point3);	/* Point3 vcs2clip(Camera*, Point3); */
		Point3 world2clip(Point3);	/* Point3 world2clip(Camera*, Point3); */
		Point3 clip2ndc(Point3);	/* Point3 clip2ndc(Point3); */
		Point3 ndc2viewport(Point3);	/* Point3 ndc2viewport(Framebuf*, Point3); */
		Point3 viewport2ndc(Point3);	/* Point3 viewport2ndc(Framebuf*, Point3); */
		Point3 ndc2vcs(Point3);		/* Point3 ndc2vcs(Camera*, Point3); */
		Point3 viewport2vcs(Point3);	/* Point3 viewport2vcs(Camera*, Point3); */
		Point3 vcs2world(Point3);	/* Point3 vcs2world(Camera*, Point3); */
		Point3 viewport2world(Point3);	/* Point3 viewport2world(Camera*, Point3); */
		Point3 world2model(Point3);	/* Point3 world2model(Entity*, Point3); */

## Examples

in libgraphics:

	Point3
	phongvshader(VSparams *sp)
	{
		Point3 pos;
		Color a, d, s;
		double ss;
	
		sp->v->n = model2world(sp->su->entity, sp->v->n);
		sp->v->p = model2world(sp->su->entity, sp->v->p);
		pos = sp->v->p;
		addvattr(sp->v, "pos", VAPoint, &pos);
		if(sp->v->mtl != nil && sp->v->mtl->normalmap != nil && sp->v->uv.w != 0){
			sp->v->tangent = model2world(sp->su->entity, sp->v->tangent);
			addvattr(sp->v, "tangent", VAPoint, &sp->v->tangent);
		}
		if(sp->v->mtl != nil){
			a = sp->v->mtl->ambient;
			d = sp->v->mtl->diffuse;
			s = sp->v->mtl->specular;
			ss = sp->v->mtl->shininess;
			addvattr(sp->v, "ambient", VAPoint, &a);
			addvattr(sp->v, "diffuse", VAPoint, &d);
			addvattr(sp->v, "specular", VAPoint, &s);
			addvattr(sp->v, "shininess", VANumber, &ss);
		}
		return world2clip(sp->su->camera, pos);
	}

in semblance:

	out point pos;
	out vector tangent;
	out color ambient;
	out color diffuse;
	out color specular;
	out double shininess;

	vs phong() {
		normal = model2world(normal);
		position = model2world(position);
		pos = position;
		if(material != nil){
			if(material->normalmap && uv.w != 0)
				tangent = model2world(tangent);
			ambient = material.ambient;
			diffuse = material.diffuse;
			specular = material.specular;
			shininess = material.shininess;
		}
		return world2clip(pos);
	}


[1]: https://shithub.us/rodri/libgraphics
[2]: https://shithub.us/rodri/renderfs
[3]: https://renderman.pixar.com/resources/RenderMan_20/shadingLanguage.html
[4]: https://www.khronos.org/files/opengles_shading_language.pdf

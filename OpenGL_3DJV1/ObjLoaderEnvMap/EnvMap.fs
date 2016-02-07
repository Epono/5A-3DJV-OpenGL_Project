#version 150

in Vertex
{
	vec3 normal;
	vec2 texcoords;
    float fresnelTerm;
    vec3 reflectedDir;
    vec3 refractedDir;
    vec3 positionWS;
} IN;

uniform vec3 u_eyePosition;

// CubeMap stockant le rendu dynamique
uniform samplerCube u_dynamicCubeMap;
// CubeMap stockant la skybox
uniform samplerCube u_cubeMap;

out vec4 Fragment;

#define USE_FRAMEBUFFER_SRGB 1

void main(void)
{
    vec3 N = normalize(IN.normal);
    vec4 ambient_env = texture(u_cubeMap, N);
    
    vec3 cubeReflectDir = normalize(IN.reflectedDir);
    vec4 env = texture(u_cubeMap, cubeReflectDir);
    vec4 local_env = texture(u_dynamicCubeMap, cubeReflectDir);
    
    vec3 cubeRefractDir = normalize(IN.refractedDir);
    vec4 transparent_env = texture(u_cubeMap, cubeRefractDir);

    // blending avec alpha premutiplie (cf. operateur "over" de Porter & Duff)
    // on interprete les valeurs RGBA de la source comme une contributation a l'image finale.
    // RGB = contribution de la couleur a l'image
    // Alpha = facteur de recouvrement. 0.0 = preserve la couleur du fond, 1.0 = recouvrement total par la source.
    //
    // comme la cubemap dynamique est effacee avec un alpha de 0.0 mais que les polygones sont opaques
    // au final la couche alpha de la cubemap dynamique agit comme un masque pour un calque.
    //
    vec4 specular_reflect = local_env + (1.0-local_env.a)*env;//vec4(pow(env.rgb, vec3(1.0/2.2)), env.a);
    vec4 specular = specular_reflect;// * IN.fresnelTerm + transparent_env * (1.0 - IN.fresnelTerm);
    vec4 ambient = ambient_env;
#if NO_GAMMA_CORRECTION || USE_FRAMEBUFFER_SRGB
    // Toutes les valeurs couleurs, y compris celles stockees dans les framebuffers, sont soient en sRGB (NO_GAMMA_CORRECTION)
    // soient entierement lineaires (USE_FRAMEBUFFER_SRGB) ce qui signifie que c'est le GPU qui va effectuer la conversion automatiquement
    // (ce qui implique que les couleurs et textures ont prealablement ete converties dans l'espace colorimetrique lineaire 
    // par exemple manuellement via pow(color, 2.2) -ou autre facteur gamma-, ou automatiquement via les formats internes GL_SRGB8 et GL_SRGB8_ALPHA8
    vec3 rgb = (/*ambient.rgb +*/ specular.rgb);
#else
    // ceci suppose que le shader d'illumination effectue les calculs dans l'espace colorimetrique lineaire.
    // donc que les textures et couleurs de materiaux/lumieres ont ete converties de valeurs corrigees gamma (sRGB)
    // vers le repere linaire (on parle alors d'une phase de "de-gamma").
    // Cela peut se faire au moment du chargement de la scene -en multipliant manuellement les couleurs par le facteur gamma
    // et/ou en utilisant GL_SRGB8 ou GL_SRGB8_ALPHA8 comme format interne pour les textures.
    // Autrement cela peut se faire au niveau des outils en appliquant le "de-gamma" aux couleurs definies par l'artiste.
    //
    // Ceci suppose egalement un framebuffer faible dynamique RGB (low dynamic range, LDR, 8 bits par composantes)
    // Il est possible de se passer de cette phase de pow(color, 1.0/2.2) en utilisant un framebuffer sRGB qui convertit
    // automatiquement les couleurs lineaires en couleurs corrigees gamma. Ceci marche tres bien pour les GPU DX10+.
    vec3 rgb = pow(/*ambient.rgb +*/ specular.rgb, vec3(1.0 / 2.2));
#endif
    float alpha = /*ambient.a +*/ specular.a;
    Fragment = vec4(rgb, alpha);
}

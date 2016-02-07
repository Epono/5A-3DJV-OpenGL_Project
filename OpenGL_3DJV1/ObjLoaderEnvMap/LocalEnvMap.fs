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


// Le principe est simple, on specifie les limites de la bounding box qui va 
// determiner quelles reflections/refractions locales doivent etre prise en compte
// puis on lance un rayon du centre de la bounding box vers 
vec3 CalcLocalReflection(vec3 R, vec3 positionWS)
{
    // Valeur en dur ! Utilisez des variables uniformes a la place 
    // note: la bounding box n'est pas necessairement un cube (parallepipede)
    // ni meme positionnee a l'origine du monde
    const vec3 bboxMin = vec3(-2.0);
    const vec3 bboxMax = vec3(2.0);
    const vec3 bboxCenter = vec3(0.0);

    // calcul de la distance (cf. algorithme d'intersection ray/box)
    vec3 intersectMin = (bboxMin - positionWS) / R;
    vec3 intersectMax = (bboxMax - positionWS) / R;
    vec3 intersect = max(intersectMin, intersectMax);
    float distance = min(min(intersect.x, intersect.y), intersect.z);

    // ray = origin + t*dir
    vec3 intersectWS = positionWS + distance * R;
    // les coordonnees de texture d'une cube map sont exprimees dans le repere local
    // Il s'agit d'une direction relative au centre de la cube map
    return intersectWS - bboxCenter;
}

// note: on peut pousser le concept plus loin en integrant dans une meme cube map les ombres
// Encore plus fort, utiliser les cube maps locales pour des reflexions poussees (comme dans Remember Me par ex.)
// ou pour calculer une light probe (eventuellement en precalculant les coefficients ambient et diffus 
// a l'aide de la technique de Spherical Harmonics, qui permet de recalculer en temps reel l'illumination avec un faible cout gpu)

#define USE_FRAMEBUFFER_SRGB 1

void main(void)
{
    // Pour une scene exterieure, la composante ambiante peut provenir de la sky box
    // en interieur on pourra sampler localement et/ou globalement selon le rendu desire
    vec3 N = normalize(IN.normal);
    vec4 ambient_env = texture(u_cubeMap, N);

    vec3 cubeReflectDir = normalize(IN.reflectedDir);
    vec3 localR = CalcLocalReflection(IN.positionWS, cubeReflectDir);
    vec4 local_env = texture(u_dynamicCubeMap, localR);    
    vec3 R = CalcLocalReflection(IN.positionWS, cubeReflectDir);
	vec4 env = texture(u_cubeMap, cubeReflectDir);

    // on peut appliquer la meme idee pour obtenir une refraction locale 
    // (je vous laisse faire cela en exercice, quasiment un simple copier/coller)
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
    vec4 specular_reflect = local_env + (1.0-local_env.a)*env;
    vec4 specular = specular_reflect;// * IN.fresnelTerm + transparent_env * (1.0 - IN.fresnelTerm);
    vec4 ambient = ambient_env;

#if NO_GAMMA_CORRECTION || USE_FRAMEBUFFER_SRGB
    // Toutes les valeurs couleurs, y compris celles stockees dans les framebuffers, sont soient en sRGB (NO_GAMMA_CORRECTION)
    // soient entierement lineaires (USE_FRAMEBUFFER_SRGB) ce qui signifie que c'est le GPU qui va effectuer la conversion automatiquement
    // (ce qui implique que les couleurs et textures ont prealablement ete converties dans l'espace colorimetrique lineaire 
    // par exemple manuellement via pow(color, 2.2) -ou autre facteur gamma-, ou automatiquement via les formats internes GL_SRGB8 et GL_SRGB8_ALPHA8
    vec3 rgb = /*ambient.rgb +*/ specular.rgb;
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

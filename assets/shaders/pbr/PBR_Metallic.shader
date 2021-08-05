Shader "Unlit/PBR_Metallic"
{
    Properties
    {
		// base color
    	_AlbedoTex ("BaseColor", 2D) = "white" {}
		_AlbedoTex_ST ("Tiling Offset", Vector) = (1.0, 1.0, 0.0, 0.0)
		_AlbedoColor ("BaseColor", Color) = (1.0, 1.0, 1.0, 1.0)
		_AlbedoIntensity ("BaseColor Intensity", range(0, 10)) = 1.0
		
		// alpha test
		_AlphaThreshold ("Alpha Threshold", range(0, 1)) = 0.5

		// normap map
        _NormalTex ("Normal Map", 2D) = "bump" {}
		_NormalTex_ST ("Tiling Offset", Vector) = (1.0, 1.0, 0.0, 0.0)
		_NormalIntensity ("Normal Scale", range(0, 10)) = 1.0

		// ocllusion + roughness + metallic
        _ParamsTex ("Occlusion Roughness Metallic", 2D) = "white" {}
		_ParamsTex_ST ("Tiling Offset", Vector) = (1.0, 1.0, 0.0, 0.0)
		_OcclusionIntensity ("Occlusion", range(0, 10)) = 1.0
		_RoughnessIntensity ("Roughness", range(0, 10)) = 1.0
		_MetallicIntensity ("Metallic", range(0, 10)) = 1.0

		// emissive
		_EmissiveTex ("Emissive", 2D) = "black" {}
		_EmissiveTex_ST ("Tiling Offset", Vector) = (1.0, 1.0, 0.0, 0.0)
		_EmissiveIntensity ("Emissive Intensity", range(0, 10)) = 1.0
		
		// sheen
		_SheenTex ("Sheen", 2D) = "black" {}
		_SheenTex_ST ("Tiling Offset", Vector) = (1.0, 1.0, 0.0, 0.0)
		_SheenColor ("SheenColor", Color) = (1.0, 1.0, 1.0, 1.0)
		_SheenRoughness ("SheenColor Intensity", range(0, 10)) = 1.0

		// clearcoat
		_ClearcoatTex ("Clearcoat", 2D) = "black" {}
		_ClearcoatTex_ST ("Tiling Offset", Vector) = (1.0, 1.0, 0.0, 0.0)
		_Clearcoat ("Clearcoat", range(0, 10)) = 1.0
		_ClearcoatRoughness ("ClearcoatRoughness", range(0, 10)) = 1.0

		// transmission
		_TransmissionTex ("Transmission", 2D) = "black" {}
		_TransmissionTex_ST ("Tiling Offset", Vector) = (1.0, 1.0, 0.0, 0.0)
		_TransmissionIntensity ("Transmission Intensity", range(0, 10)) = 1.0

		// exposure
		_Exposure ("Exposure", Range(0.1, 10)) = 1.0
		_GammaValue ("Gamma", Range(0.1, 10)) = 2.2

		// antialiasing
		_specularAntiAliasingVariance ("AntiAliasingVariance", range(0, 10)) = 0.15
		_specularAntiAliasingThreshold ("AntiAliasingThreshold", range(0, 10)) = 0.20

		_PeelBias ("Peel Depth Bias", range(0, 0.1)) = 0.001
    }

    SubShader
    {
        LOD 100

		Pass
		{
			CGPROGRAM

			#pragma vertex vert
			#pragma fragment frag
			#pragma glsl_multi_compile SKELETON ALPHA_TEST ALPHA_BLEND SHADOW

			#include "UnityCG.cginc"
			#include "Lighting.cginc"
			// muse骨骼库
			#include "MuseSkeleton.cginc"
			// muse pbr库
			#include "BRDF.cginc"
			#include "ToneMapping.cginc"
			#include "IBL.cginc"
			#include "Punctual.cginc"
			
			struct appdata
			{
				float4 vertex  : POSITION;
				float2 uv      : TEXCOORD0;
				float3 normal  : NORMAL;
				float4 tangent : TANGENT;
			#ifdef SKELETON
				float4 indices : SKIN_INDICES;
                float4 weights : SKIN_WEIGHTS;
			#endif
			};

			struct v2f
			{
				float4 vertex         : POSITION;
				float2 uv             : TEXCOORD0;
				// world space的坐标点
				float3 worldPos       : TEXCOORD1;
				// 切线相关
				float3 tspace0        : TEXCOORD2;
				float3 tspace1        : TEXCOORD3;
				float3 tspace2        : TEXCOORD4;
				// ibl相关
				float3 envIrradiance  : TEXCOORD5;
				// clip space
				float4 clipCoords     : TEXCOORD6;
			};

			struct FragOutput
			{
				float4 color : SV_Target;
			#ifdef ALPHA_BLEND
				float  depth : DEPTH;
			#endif
			};

			float       _specularAntiAliasingVariance;
			float       _specularAntiAliasingThreshold;
			
			// base color
			sampler2D   _AlbedoTex;
			float4		_AlbedoTex_ST;
			float4		_AlbedoColor;
			float		_AlbedoIntensity;

			// sheen
			sampler2D   _SheenTex;
			float4		_SheenTex_ST;
			float4		_SheenColor;
			float		_SheenRoughness;

			// clear coat
			sampler2D   _ClearcoatTex;
			float4      _ClearcoatTex_ST;
			float		_Clearcoat;
			float		_ClearcoatRoughness;

			// transmission
			sampler2D	_TransmissionTex;
			float4      _TransmissionTex_ST;
			float		_TransmissionIntensity;

			float		_AlphaThreshold;

            sampler2D   _NormalTex;
			float4		_NormalTex_ST;
			float 		_NormalIntensity;

            sampler2D   _ParamsTex;
			float4		_ParamsTex_ST;
			float		_OcclusionIntensity;
			float		_RoughnessIntensity;
			float		_MetallicIntensity;

			sampler2D	_EmissiveTex;
			float4		_EmissiveTex_ST;
			float		_EmissiveIntensity;

		#ifdef SHADOW
			sampler2D   _ShadowMask;
		#endif

		#ifdef ALPHA_BLEND
			sampler2D 	_PeelDepth;
			float 		_PeelLayer;
			float 		_PeelBias;
		#endif

			v2f vert (appdata v)
			{
				v2f o;
				
				float3 vertex  = v.vertex.xyz;
				float3 normal  = v.normal.xyz;
				float3 tangent = v.tangent.xyz;

				// 应用骨骼动画
			#ifdef SKELETON
				SkinVertex skinVertex = TrasnformSkinVertex(v.indices, v.weights, vertex, normal, tangent);
				vertex  = skinVertex.vertex;
				normal  = skinVertex.normal;
				tangent = skinVertex.tangent;
			#endif

				// normalize
				normal  = normalize(normal);
				tangent = normalize(tangent);

				// 计算worldspace的法线以及切线
				float3 wNormal    = UnityObjectToWorldNormal(normal);
				float3 wTangent   = UnityObjectToWorldDir(tangent);
				float3 wBitangent = cross(wNormal, wTangent) * v.tangent.w;

				// TBN
				o.tspace0 = float3(wTangent.x, wBitangent.x, wNormal.x);
                o.tspace1 = float3(wTangent.y, wBitangent.y, wNormal.y);
                o.tspace2 = float3(wTangent.z, wBitangent.z, wNormal.z);

				// clipspace坐标、uv、worldspace坐标
				o.vertex   = UnityObjectToClipPos(vertex);
				o.uv       = v.uv;
				o.worldPos = mul(unity_ObjectToWorld, float4(vertex, 1.0)).xyz;

				// clip space
				o.clipCoords = o.vertex;

				return o;
			}
			
			struct MaterialInfo
			{
				float perceptualRoughness; 
				float3 f0;                    

				float alphaRoughness;         
				float3 albedoColor;

				float3 f90;                   
				float metallic;

				float3 n;
				float3 baseColor; 

				float sheenRoughnessFactor;
				float3 sheenColorFactor;

				float3 clearcoatF0;
				float3 clearcoatF90;
				float clearcoatFactor;
				float3 clearcoatNormal;
				float clearcoatRoughness;

				float transmissionFactor;
			};

			float NormalFiltering(float perceptualRoughness, float3 worldNormal) 
			{
				float3 du = ddx(worldNormal);
				float3 dv = ddy(worldNormal);

				float variance  = _specularAntiAliasingVariance * (dot(du, du) + dot(dv, dv));
				float roughness = perceptualRoughness * perceptualRoughness;

				float kernelRoughness = min(2.0 * variance, _specularAntiAliasingThreshold);
				float squareRoughness = clamp(roughness * roughness + kernelRoughness, 0, 1);

				return sqrt(sqrt(squareRoughness));
			}

			MaterialInfo GetMetallicRoughnessInfo(MaterialInfo info, float3 worldNormal, float f0_ior, float4 mrSample)
			{
				info.metallic = _MetallicIntensity;
				info.metallic *= mrSample.b;

				info.perceptualRoughness  = _RoughnessIntensity;
				info.perceptualRoughness *= mrSample.g;
				info.perceptualRoughness  = NormalFiltering(info.perceptualRoughness, worldNormal);
				info.perceptualRoughness  = clamp(info.perceptualRoughness, 0.045, 1.0);

				float3 f0 = float3(f0_ior);
	
				info.albedoColor = lerp(info.baseColor.rgb * (float3(1.0) - f0),  float3(0), info.metallic);
				info.f0 = lerp(f0, info.baseColor.rgb, info.metallic);

				return info;
			}

			MaterialInfo GetSheenInfo(MaterialInfo info, float4 sheenColorSample)
			{
				info.sheenColorFactor = _SheenColor;
				info.sheenColorFactor *= sheenColorSample.rgb;

				info.sheenRoughnessFactor = _SheenRoughness;
				info.sheenRoughnessFactor *= sheenColorSample.a;

				return info;
			}

			MaterialInfo GetClearCoatInfo(MaterialInfo info, float3 ng, float f0_ior, float4 clearcoatSampler)
			{
				info.clearcoatFactor = _Clearcoat;
				info.clearcoatFactor *= clearcoatSampler.r;

				info.clearcoatRoughness = _ClearcoatRoughness;
				info.clearcoatRoughness *= clearcoatSampler.g;
				info.clearcoatRoughness = clamp(info.clearcoatRoughness, 0.0, 1.0);

				info.clearcoatF0 = float3(f0_ior);
				info.clearcoatF90 = float3(1.0);

				info.clearcoatNormal = ng;

				return info;
			}

			MaterialInfo GetTransmissionInfo(MaterialInfo info, float4 transmissionSample)
			{
				info.transmissionFactor = _TransmissionIntensity;
				info.transmissionFactor *= transmissionSample.a;

				return info;
			}

			FragOutput frag (v2f i)
			{
				FragOutput o;

				float4 projCoord = i.clipCoords / i.clipCoords.w;
				projCoord.xy = projCoord.xy * 0.5 + 0.5;

		#ifdef ALPHA_BLEND
				if (_PeelLayer >= 1) {
					float depth = tex2D(_PeelDepth, projCoord.xy).r * 2.0 - 1.0;
					if (depth + _PeelBias > projCoord.z) {
						discard;
					}
					o.depth = (projCoord.z + 1.0 + _PeelBias) * 0.5;
				}
				else {
					o.depth = (projCoord.z + 1.0) * 0.5;
				}
		#endif

				// albedo texture
                float4 albedoTexColor = tex2D(_AlbedoTex, i.uv * _AlbedoTex_ST.xy + _AlbedoTex_ST.zw);
				
				// albedo color
				float4 surfaceAlbedo = _AlbedoColor;
				surfaceAlbedo.rgb *= sRGBToLinear(albedoTexColor.rgb);
				surfaceAlbedo.rgb *= _AlbedoIntensity;
				surfaceAlbedo.a   *= albedoTexColor.a;

				// alpha test
			#ifdef ALPHA_TEST
				if (surfaceAlbedo.a < _AlphaThreshold) {
					discard;
				}
			#endif

				// shadow
				float shadow = 1.0;
			#ifdef SHADOW
					shadow = tex2D(_ShadowMask, projCoord.xy).r;
					shadow = clamp(shadow, 0.0, 1.0);
			#endif

				// normal
				float3 tnormal = UnpackNormal(tex2D(_NormalTex, i.uv * _NormalTex_ST.xy + _NormalTex_ST.zw));
				tnormal *= float3(_NormalIntensity, _NormalIntensity, 1.0);
				float3 worldNormal = float3(dot(i.tspace0, tnormal), dot(i.tspace1, tnormal), dot(i.tspace2, tnormal));

				// N、V、R
				float3 n = normalize(worldNormal);
				float3 v = normalize(_WorldSpaceCameraPos.xyz - i.worldPos.xyz);
				float3 r = reflect(-v, n);

				// ior
				float ior = 1.5;
    			float f0_ior = 0.04;

				// Occlusion in Red、Roughness in Green、Metallic in Blue
				float4 paramsTexColor = tex2D(_ParamsTex, i.uv * _ParamsTex_ST.xy + _ParamsTex_ST.zw);

				// material
				MaterialInfo materialInfo;
    			materialInfo.baseColor = surfaceAlbedo.rgb;

				// metallic roughness
				materialInfo = GetMetallicRoughnessInfo(materialInfo, float3(i.tspace0.z, i.tspace1.z, -i.tspace2.z), f0_ior, paramsTexColor);

				// sheen
				materialInfo = GetSheenInfo(materialInfo, tex2D(_SheenTex, i.uv * _SheenTex_ST.xy + _SheenTex_ST.zw));

				// clearcoat
				materialInfo = GetClearCoatInfo(materialInfo, float3(i.tspace0.z, i.tspace1.z, i.tspace2.z), f0_ior, tex2D(_ClearcoatTex, i.uv * _ClearcoatTex_ST.xy + _ClearcoatTex_ST.zw));
				
				// transmission
				materialInfo = GetTransmissionInfo(materialInfo, tex2D(_TransmissionTex, i.uv * _TransmissionTex_ST.xy + _TransmissionTex_ST.zw));

				// squaring Roughness
				materialInfo.perceptualRoughness = clamp(materialInfo.perceptualRoughness, 0.0, 1.0);
    			materialInfo.metallic = clamp(materialInfo.metallic, 0.0, 1.0);
				materialInfo.alphaRoughness = materialInfo.perceptualRoughness * materialInfo.perceptualRoughness;

				// Compute reflectance. less than 2% is physically impossible
				float reflectance = max(max(materialInfo.f0.r, materialInfo.f0.g), materialInfo.f0.b);
				materialInfo.f90 = float3(clamp(reflectance * 50.0, 0.0, 1.0));
				materialInfo.n = n;

				// LIGHTING
				float3 f_specular     = float3(0.0);
				float3 f_diffuse      = float3(0.0);
				float3 f_emissive     = float3(0.0);
				float3 f_clearcoat    = float3(0.0);
				float3 f_sheen        = float3(0.0);
				float3 f_transmission = float3(0.0);
				float albedoSheenScaling = 1.0;

				// IBL
				f_specular += GetIBLRadianceGGX(n, v, materialInfo.perceptualRoughness, materialInfo.f0);
				f_diffuse  += GetIBLRadianceLambertian(n, materialInfo.albedoColor);

				// sheen
				if (dot(materialInfo.sheenColorFactor, materialInfo.sheenColorFactor) > 0)
				{
					f_sheen += GetIBLRadianceCharlie(n, v, materialInfo.sheenRoughnessFactor, materialInfo.sheenColorFactor);
				}

				// clear coat
				if (materialInfo.clearcoatFactor > 0)
				{	
					f_clearcoat += GetIBLRadianceGGX(materialInfo.clearcoatNormal, v, materialInfo.clearcoatRoughness, materialInfo.clearcoatF0);
				}

				// transmission
				if (materialInfo.transmissionFactor > 0)
				{
					f_transmission += GetIBLRadianceTransmission(n, v, materialInfo.perceptualRoughness, materialInfo.baseColor, materialInfo.f0, materialInfo.f90);
				}
				
				// ao
				float ao  = paramsTexColor.r;
    			f_diffuse = lerp(f_diffuse, f_diffuse * ao * shadow, _OcclusionIntensity);

				// direction light
				float rangeAttenuation = 1.0;
        		float spotAttenuation  = 1.0;
				float3 intensity = rangeAttenuation * spotAttenuation * _LightColor0.rgb;

				float3 l = normalize(_WorldSpaceLightPos0.xyz);
				float3 h = normalize(l + v);
				float NdotL = ClampedDot(n, l);
				float NdotV = ClampedDot(n, v);
				float NdotH = ClampedDot(n, h);
				float LdotH = ClampedDot(l, h);
				float VdotH = ClampedDot(v, h);

				if (NdotL > 0.0 || NdotV > 0.0)
				{
					f_diffuse  += intensity * NdotL * BRDF_lambertian(materialInfo.f0, materialInfo.f90, materialInfo.albedoColor, VdotH);
					f_specular += intensity * NdotL * BRDF_specularGGX(materialInfo.f0, materialInfo.f90, materialInfo.alphaRoughness, VdotH, NdotL, NdotV, NdotH);

					// sheen
					if (dot(materialInfo.sheenColorFactor, materialInfo.sheenColorFactor) > 0)
					{
						f_sheen += intensity * GetPunctualRadianceSheen(materialInfo.sheenColorFactor, materialInfo.sheenRoughnessFactor, NdotL, NdotV, NdotH);
                		albedoSheenScaling = min(1.0 - Max3(materialInfo.sheenColorFactor) * AlbedoSheenScalingLUT(NdotV, materialInfo.sheenRoughnessFactor), 1.0 - Max3(materialInfo.sheenColorFactor) * AlbedoSheenScalingLUT(NdotL, materialInfo.sheenRoughnessFactor));
					}
					
					// clearcoat
					if (materialInfo.clearcoatFactor > 0)
					{
						f_clearcoat += intensity * GetPunctualRadianceClearCoat(materialInfo.clearcoatNormal, v, l, h, VdotH, materialInfo.clearcoatF0, materialInfo.clearcoatF90, materialInfo.clearcoatRoughness);
					}
				}

				// transmission
				if (materialInfo.transmissionFactor > 0)
				{
					f_transmission += intensity * GetPunctualRadianceTransmission(n, v, l, materialInfo.alphaRoughness, materialInfo.f0, materialInfo.f90, materialInfo.transmissionFactor, materialInfo.baseColor);
				}
				
				// emissive
				f_emissive = tex2D(_EmissiveTex, i.uv * _EmissiveTex_ST.xy + _EmissiveTex_ST.zw).xyz;
				f_emissive.rgb *= sRGBToLinear(f_emissive.rgb);
				f_emissive.rgb *= _EmissiveIntensity;

				// clear coat
				float clearcoatFactor = 0.0;
        		float3 clearcoatFresnel = float3(0.0);
				if (materialInfo.clearcoatFactor > 0)
				{
					clearcoatFactor  = materialInfo.clearcoatFactor;
					clearcoatFresnel = F_Schlick(materialInfo.clearcoatF0, materialInfo.clearcoatF90, ClampedDot(materialInfo.clearcoatNormal, v));
				}
				
				// transmission
				float3 diffuse = f_diffuse;
				if (materialInfo.transmissionFactor > 0)
				{
					diffuse = lerp(f_diffuse, f_transmission, materialInfo.transmissionFactor);
				}

				// final color
				float3 color = float3(0);
				color = f_emissive + diffuse + f_specular;
				color = f_sheen + color * albedoSheenScaling;
				color = color * (1.0 - clearcoatFactor * clearcoatFresnel) + f_clearcoat * clearcoatFactor;

				o.color = float4(ToneMap(color), surfaceAlbedo.a);

				return o;
			}

			ENDCG
		}
		
        Pass
        {
			Tags { "LightMode"="ShadowCaster" }

            CGPROGRAM

            #pragma vertex vert
            #pragma fragment frag
			#pragma glsl_multi_compile SKELETON ALPHA_TEST

            #include "UnityCG.cginc"
			#include "MuseSkeleton.cginc"
			#include "MusePBR.cginc"
			
            struct appdata
            {
                float4 vertex  : POSITION;

			#ifdef SKELETON
				float4 indices : SKIN_INDICES;
                float4 weights : SKIN_WEIGHTS;
			#endif

			#ifdef ALPHA_TEST
				float2 uv      : TEXCOORD0;
			#endif
            };

            struct v2f
            {
				float4 vertex  : SV_POSITION;

			#ifdef ALPHA_TEST
				float2 uv      : TEXCOORD0;
			#endif
            };

            v2f vert (appdata v)
            {
                v2f o;
				
				float3 vertex = v.vertex;

			#ifdef SKELETON
				vertex = TrasnformSkinPosition(v.indices, v.weights, vertex);
			#endif
				
				o.vertex = UnityObjectToClipPos(float4(vertex.xyz, 1.0));

			#ifdef ALPHA_TEST
				o.uv = v.uv;
			#endif

                return o;
            }

		#ifdef ALPHA_TEST
			sampler2D   _AlbedoTex;
			float4		_AlbedoTex_ST;
			float4		_AlbedoColor;
			float		_AlphaThreshold;
		#endif
			
            float4 frag (v2f i) : SV_Target
            {
			#ifdef ALPHA_TEST
				float4 albedoTexColor = tex2D(_AlbedoTex, i.uv * _AlbedoTex_ST.xy + _AlbedoTex_ST.zw);
				float4 surfaceAlbedo = _AlbedoColor;
				surfaceAlbedo.a *= albedoTexColor.a;
				if (surfaceAlbedo.a < _AlphaThreshold) {
					discard;
				}
			#endif
                return float4(0, 0, 0, 0);
            }

            ENDCG
        }

    }
}

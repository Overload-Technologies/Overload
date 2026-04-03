# Skinned Mesh Animation Implementation

This document describes the current skeletal animation and skinning integration added to Overload.

## Scope

The implementation adds runtime skeletal animation playback for skinned models, GPU skinning support at render time, and editor selection outline support for animated meshes.

## What Was Added

### Rendering Data

- `OvRendering::Animation` data model:
  - `Skeleton`
  - `Bone`
  - `SkeletonNode`
  - `SkeletalAnimation`
  - `NodeAnimationTrack`
  - `Keyframe<T>`
- Vertex format extension:
  - Bone IDs (`vec4` as 4 floats)
  - Bone weights (`vec4`)
- Model resource extension:
  - Optional skeleton
  - Animation clip list
  - `IsSkinned()`, `GetSkeleton()`, `GetAnimations()`, `FindAnimation()`

### Import / Parsing

- Assimp parser now builds:
  - Skeleton hierarchy from scene nodes
  - Bone map and offset matrices
  - Animation clips and node tracks
  - Per-vertex bone influences (normalized)
- Parser safeguards:
  - Forces `TRIANGULATE`
  - Forces `LIMIT_BONE_WEIGHTS`
  - Disables `PRE_TRANSFORM_VERTICES`
  - Disables `DEBONE`

### Runtime / ECS

- New ECS component:
  - `CSkinnedMeshRenderer`
- Features:
  - Clip selection by index/name
  - Playback controls (`Play`, `Pause`, `Stop`)
  - Looping
  - Playback speed (including reverse playback with negative speed)
  - Time control in seconds
  - Pose evaluation rate limiter
  - Bone matrix palette generation (and transposed palette for GPU upload)
- Integration with `CModelRenderer`:
  - Skinned renderer is auto-created when a skinned model is assigned
  - Component is synchronized when model changes

### Render Pipeline

- New descriptor:
  - `SkinningDrawableDescriptor` (palette pointer, matrix count, pose version)
- New render feature:
  - `SkinningRenderFeature`
  - Uploads and binds skinning palette to SSBO binding point `2`
  - Uses ring buffers + identity fallback palette
- New helper module:
  - `SkinningUtils` for feature set and descriptor application
- Scene integration:
  - Skinning descriptor attached during scene parsing
  - `featureSetOverride` adds `SKINNING` when shader/material supports it

### Shader Integration

- Added shared shader include:
  - `Resources/Engine/Shaders/Common/Buffers/SkinningSSBO.ovfxh`
- Added `SKINNING` variants in:
  - `Resources/Engine/Shaders/Standard.ovfx`
  - `Resources/Engine/Shaders/Unlit.ovfx`
- Editor outline integration:
  - `Resources/Editor/Shaders/OutlineFallback.ovfx` includes:
    - `#pass OUTLINE_PASS`
    - `#feature SKINNING`
    - Skinning path in vertex shader
  - `OutlineRenderFeature` passes skinning descriptor + feature override for selected skinned actors

## What You Can Do With It

- Load and render skinned animated models.
- Select an active animation clip at runtime.
- Control animation speed and time.
- Render animated geometry with GPU skinning.
- Display selection outlines that follow the current animated pose (not bind pose / T-pose).

## Current Limits

- Maximum bone influences per vertex: **4** (`kMaxBonesPerVertex = 4`).
- Single active clip per `CSkinnedMeshRenderer`:
  - No clip blending
  - No state machine
  - No layered animation
- No root motion extraction system.
- No IK / retargeting features.
- Frustum culling behavior for skinned drawables:
  - Dynamic skinned deformation can exceed static mesh bounds
  - To avoid missing parts (for example hands), frustum sphere culling is skipped for skinned drawables in `SceneRenderer::FilterDrawables`
  - This is conservative and can increase draw cost for animated actors

## Performance Notes

- Skinning upload path is optimized with:
  - Pose version tracking
  - Upload only when palette changed
  - Ring-buffered SSBO allocation strategy
- CPU pose evaluation frequency can be reduced via `Pose Eval Rate`.
- The implementation prioritizes correctness and stability over aggressive culling for skinned meshes.

## Quick Usage

1. Assign a skinned model to `CModelRenderer`.
2. `CSkinnedMeshRenderer` is attached/synchronized automatically.
3. In inspector:
   - Choose animation clip
   - Enable/disable playback
   - Set looping/speed/time
4. Ensure target materials use shaders with `SKINNING` support (`Standard`, `Unlit`, or custom shaders with the same SSBO contract).

## Future Improvements

- Animation blending (cross-fade and layered blending).
- Animator graph / state machine.
- Optional dynamic skinned bounds update for better culling precision.
- Retargeting utilities and root motion support.
- Compute-skinning path for very large animated crowds.


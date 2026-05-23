AssertTrue(Key.UNKNOWN ~= nil, "Key.UNKNOWN is registered")
AssertTrue(Key.SPACE ~= nil, "Key.SPACE is registered")
AssertTrue(Key.A == Key.A, "Key equality is stable")
AssertTrue(Key.A ~= Key.B, "Key inequality is stable")

AssertTrue(MouseButton.BUTTON_1 ~= nil, "MouseButton.BUTTON_1 is registered")
AssertTrue(MouseButton.BUTTON_LEFT == MouseButton.BUTTON_1, "left mouse button alias")
AssertTrue(MouseButton.BUTTON_RIGHT == MouseButton.BUTTON_2, "right mouse button alias")
AssertTrue(MouseButton.BUTTON_MIDDLE == MouseButton.BUTTON_3, "middle mouse button alias")

AssertTrue(CollisionDetectionMode.DISCRETE ~= nil, "CollisionDetectionMode.DISCRETE is registered")
AssertTrue(CollisionDetectionMode.DISCRETE ~= CollisionDetectionMode.CONTINUOUS, "collision modes are distinct")

AssertTrue(ProjectionMode.ORTHOGRAPHIC ~= nil, "ProjectionMode.ORTHOGRAPHIC is registered")
AssertTrue(ProjectionMode.ORTHOGRAPHIC ~= ProjectionMode.PERSPECTIVE, "projection modes are distinct")

AssertTrue(FrustumBehaviour.DISABLED ~= nil, "FrustumBehaviour.DISABLED is registered")
AssertTrue(FrustumBehaviour.DISABLED ~= FrustumBehaviour.MESH_BOUNDS, "frustum behaviours are distinct")

AssertTrue(TonemappingMode.NEUTRAL ~= nil, "TonemappingMode.NEUTRAL is registered")
AssertTrue(TonemappingMode.NEUTRAL ~= TonemappingMode.ACES, "tonemapping modes are distinct")

AssertTrue(ReflectionProbeRefreshMode.REALTIME ~= nil, "ReflectionProbeRefreshMode.REALTIME is registered")
AssertTrue(ReflectionProbeRefreshMode.REALTIME ~= ReflectionProbeRefreshMode.ONCE, "reflection refresh modes are distinct")

AssertTrue(ReflectionProbeCaptureSpeed.ONE_FACE ~= nil, "ReflectionProbeCaptureSpeed.ONE_FACE is registered")
AssertTrue(ReflectionProbeCaptureSpeed.ONE_FACE ~= ReflectionProbeCaptureSpeed.SIX_FACES, "reflection capture speeds are distinct")

AssertTrue(ReflectionProbeInfluencePolicy.GLOBAL ~= nil, "ReflectionProbeInfluencePolicy.GLOBAL is registered")
AssertTrue(ReflectionProbeInfluencePolicy.GLOBAL ~= ReflectionProbeInfluencePolicy.LOCAL, "reflection influence policies are distinct")

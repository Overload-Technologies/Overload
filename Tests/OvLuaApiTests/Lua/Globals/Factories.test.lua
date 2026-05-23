local model = Model()
AssertEquals("userdata", type(model), "Model factory")
AssertEquals("", model.path, "Model default path")
model.path = "Assets/Models/Ship.fbx"
AssertEquals("Assets/Models/Ship.fbx", model.path, "Model path mutation")

local texture = Texture()
AssertEquals("userdata", type(texture), "Texture factory")
AssertEquals("", texture.path, "Texture default path")

local shader = Shader()
AssertEquals("userdata", type(shader), "Shader factory")
AssertEquals("", shader.path, "Shader default path")

local material = Material()
AssertEquals("userdata", type(material), "Material factory")
AssertEquals("", material.path, "Material default path")

local sound = Sound()
AssertEquals("userdata", type(sound), "Sound factory")
AssertEquals("", sound.path, "Sound default path")

local prefab = Prefab()
AssertEquals("userdata", type(prefab), "Prefab factory")
AssertEquals("", prefab.path, "Prefab default path")

local actor = Actor()
AssertEquals("userdata", type(actor), "Actor factory")
AssertEquals(0, actor.guid, "Actor default guid")
actor.guid = 42
AssertEquals(42, actor.guid, "Actor guid mutation")

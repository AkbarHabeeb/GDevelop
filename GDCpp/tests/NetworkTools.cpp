/*
 * GDevelop C++ Platform
 * Copyright 2008-2016 Florian Rival (Florian.Rival@gmail.com). All rights
 * reserved. This project is released under the MIT License.
 */
/**
 * @file Tests covering network features and JSON serialization.
 */
#include "GDCpp/Extensions/Builtin/NetworkTools.h"
#include "GDCore/CommonTools.h"
#include "GDCore/Project/ObjectsContainer.h"
#include "GDCore/Project/Layout.h"
#include "GDCpp/Runtime/RuntimeGame.h"
#include "GDCpp/Runtime/RuntimeScene.h"
#include "catch.hpp"

TEST_CASE("NetworkTools", "[game-engine]") {
  SECTION("gd::Variable from/to JSON conversions") {
    SECTION("Basics") {
      gd::Variable var;
      gd::String originalJSON = "{\"ok\": true,\"hello\": \"world\"}";
      JSONToVariableStructure(originalJSON, var);
      REQUIRE(VariableStructureToJSON(var) ==
              "{\"hello\": \"world\",\"ok\": 1}");
    }

    SECTION("Quotes and special characters") {
      gd::Variable var;
      gd::String originalJSON =
          "{\"\\\"hello\\\"\": \" \\\"quote\\\" \",\"caret-prop\": "
          "1,\"special-\\b\\f\\n\\r\\t\\\"\": \"\\b\\f\\n\\r\\t\"}";
      JSONToVariableStructure(originalJSON, var);
      REQUIRE(VariableStructureToJSON(var) == originalJSON);
    }

    SECTION("Array") {
      // For gd::Variables, arrays are converted to properties called "0",
      // "1"...
      gd::Variable var;
      gd::String originalJSON =
          "[\"Hello \\\"you\\\"\", 42, {\"a\": \"world\"}]";
      JSONToVariableStructure(originalJSON, var);
      REQUIRE(var.GetChild("0").GetString() == "Hello \"you\"");
      REQUIRE(var.GetChild("1").GetValue() == 42);
      REQUIRE(var.GetChild("2").GetChild("a").GetString() == "world");

      REQUIRE(
          VariableStructureToJSON(var) ==
          "{\"0\": \"Hello \\\"you\\\"\",\"1\": 42,\"2\": {\"a\": \"world\"}}");
    }
  }
}

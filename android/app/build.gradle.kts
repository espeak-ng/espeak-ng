import java.io.FileInputStream
import java.util.Properties
import org.gradle.crypto.checksum.Checksum

plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.crypto.checksum)
}

val keystorePropertiesFile = rootProject.file("keystore.properties")
val useKeystoreProperties = keystorePropertiesFile.canRead()
val keystoreProperties = Properties()
if (useKeystoreProperties) {
    keystoreProperties.load(FileInputStream(keystorePropertiesFile))
}

java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(17))
    }
}

android {
    if (useKeystoreProperties) {
        signingConfigs {
            create("release") {
                keyAlias = keystoreProperties["keyAlias"] as String
                keyPassword = keystoreProperties["keyPassword"] as String
                storeFile = file(keystoreProperties["storeFile"]!!)
                storePassword = keystoreProperties["storePassword"] as String
enableV1Signing = true
                enableV2Signing = true
                enableV3Signing = true
                enableV4Signing = true
            }
        }
    }

    compileSdk = 34
    buildToolsVersion = "34.0.0"
    ndkVersion = "26.3.11579264"
    namespace = "com.reecedunn.espeak"

    defaultConfig {
        applicationId = "com.reecedunn.espeak"
        minSdk = 21
        targetSdk = 33
        versionCode = 22
        versionName = "1.52-dev"

        testInstrumentationRunner = "android.test.InstrumentationTestRunner"
        testApplicationId = "com.reecedunn.espeak.test"
        ndk {
            abiFilters += listOf(
                "arm64-v8a"
            )
        }
        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DUSE_ASYNC:BOOL=OFF",
                    "-DUSE_MBROLA:BOOL=OFF"
                )
                targets += listOf(
                    "ttsespeak",
                    "espeak-data"
                )
            }
        }
    }

    buildTypes {
        debug {
            applicationIdSuffix = ".debug"
            versionNameSuffix = "-DEBUG"
            resValue("string", "app_name", "eSpeak debug")
        }
        release {
            isMinifyEnabled = false
            isShrinkResources = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
            if (useKeystoreProperties) {
                signingConfig = signingConfigs.getByName("release")
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    lint {
        abortOnError = false
    }

    compileOptions {
        sourceCompatibility(JavaVersion.VERSION_17)
        targetCompatibility(JavaVersion.VERSION_17)
    }
    buildFeatures {
        buildConfig = true
    }
}

dependencies {
    implementation(libs.androidx.appcompat)
    androidTestImplementation(libs.android.test)
    androidTestImplementation(libs.hamcrest)
}

tasks.register("checkData") {
    dependsOn("externalNativeBuildDebug")
    doFirst {
        val dataFiles = listOf(
            "en_dict",
            "intonations",
            "phondata",
            "phondata-manifest",
            "phonindex",
            "phontab"
        )
        dataFiles.forEach {
            if (!file(layout.buildDirectory.dir("generated/espeak-ng-data/$it")).exists()) {
                throw GradleException("Data file $it not found.")
            }
        }
    }
}

val dataArchive by tasks.register<Zip>("createDataArchive") {
    dependsOn("checkData")
    isPreserveFileTimestamps = false
    isReproducibleFileOrder = true
    archiveFileName.set("espeakdata.zip")
    destinationDirectory.set(file("src/main/res/raw"))

    from(layout.buildDirectory.dir("generated/espeak-ng-data"))
    into("espeak-ng-data")
}

val dataHash by tasks.register<Checksum>("createDataHash") {
    dependsOn(dataArchive)
    checksumAlgorithm.set(Checksum.Algorithm.SHA256)
    inputFiles.setFrom(dataArchive)
    outputDirectory.set(layout.buildDirectory.dir("intermediates/datahash"))
}

tasks.register<Copy>("createDataVersion") {
    dependsOn(dataHash)
    from(dataHash)
    rename("espeakdata.zip.sha256", "espeakdata_version")
    into(file("./src/main/res/raw"))
}

project.afterEvaluate {
    val tasksDependingOnCreateDataVersion = listOf(
        "generateDebugResources",
        "generateReleaseResources",
        "mapDebugSourceSetPaths",
        "mapReleaseSourceSetPaths"
    )

    tasksDependingOnCreateDataVersion.forEach { taskName ->
        tasks.named(taskName) {
            dependsOn("createDataVersion")
        }
    }
}

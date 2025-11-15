#include <windows.h>
#include <mutex>
#include <filesystem>
#include <format>
#include <toml++/toml.hpp>

#include "nya_commonhooklib.h"
#include "nfsps.h"

#include "include/chloemenulib.h"

uint32_t nFECarSetter = 0;
uint32_t nLastPreviewedCar = 0;
void OnWorldService() {
	if (nFECarSetter) {
		FeGarageMain::SetRideInfo(FeGarageMain::spInstance, nFECarSetter, BLUEPRINT1);
		nFECarSetter = 0;
	}
}

std::string lastState;

// clear all previous AI cars made by the mod
void ClearAllCreatedAICars() {
	auto cars = &UserProfile::spUserProfiles[0]->mCarStable;
	for (int i = 0; i < 410; i++) {
		auto car = &cars->CarTable[i];
		if (car->FilterBits == 0xF6910) {
			memset(car,0,sizeof(*car));
			car->Handle = 0xFFFFFFFF;
		}
	}
}

// screenshot cars use different car variations that don't exist for FE cars(?)
struct tScreenshotCarHack {
	std::string name;
	std::string modelName;
};
tScreenshotCarHack aScreenshotCars[] = {
		{"screenshot_911t_grip_1", "997tt"},
		{"screenshot_ae86_grip_1", "corolla"},
		{"screenshot_corvette_grip_1", "corvette"},
		{"screenshot_cuda_grip_1", "cuda"},
		{"screenshot_elise_grip_1", "elise"},
		{"screenshot_g35_grip_1", "g35"},
		{"screenshot_gt500_grip_1", "mustangshlbyn"},
		{"screenshot_is350_grip_1", "is350"},
		{"screenshot_kobel_s15_grip_1", "silvia"},
		{"screenshot_mustang_grip_1", "mustanggt"},
		{"screenshot_rsx_grip_1", "rsx"},
		{"screenshot_rx7_grip_1", "rx7"},
		{"screenshot_rx8_grip_1", "rx8"},
		{"screenshot_s15_grip_1", "silvia"},
		{"screenshot_skyline_grip_1", "skyline"},
		{"screenshot_supra_grip_1", "supra"},
};

bool CreateAndPreviewAICar(uint32_t hash) {
	if (!Attrib::FindCollection(0x27E73952, hash)) return false;

#ifdef SCREENSHOT_CAR_HACK
	uint32_t screenshotHackOldHash;
	uint32_t screenshotHackModel = 0;
	for (auto& mdl : aScreenshotCars) {
		if (hash == Attrib::StringHash32(mdl.name.c_str())) {
			screenshotHackOldHash = hash;
			screenshotHackModel = Attrib::StringHash32(mdl.modelName.c_str());
		}
	}
#endif

	auto cars = &UserProfile::spUserProfiles[0]->mCarStable;
	if (auto car = FEPlayerCarDB::GetCarRecordByHandle(cars, hash)) {
		nFECarSetter = nLastPreviewedCar = car->Handle;
		lastState = "Previewing car from records";
		return true;
	}

	if (auto car = FEPlayerCarDB::CreateNewPresetCar(cars, hash)) {
		ClearAllCreatedAICars();
		nFECarSetter = nLastPreviewedCar = car->Handle;
		car->FilterBits = 0xF6910;
		lastState = "Previewing car from presets";
		return true;
	}
	lastState = "Failed to create preset car";
	return false;
}

void AddCarToCareer(uint32_t handle) {
	auto cars = &UserProfile::spUserProfiles[0]->mCarStable;
	if (auto out = cars->CreateNewCustomCar(cars, handle)) {
		if (auto customization = cars->GetCustomizationRecordByHandle(cars, out->Customization)) {
			customization->mBlueprintIsLocked[0] = customization->mBlueprintIsLocked[1] = customization->mBlueprintIsLocked[2] = false;
		}
		lastState = "Car added to career";
		return;
	}
	lastState = "Failed to add car to career";
}

void AddLastAICarToCareer() {
	auto cars = &UserProfile::spUserProfiles[0]->mCarStable;

	auto record = FEPlayerCarDB::GetCarRecordByHandle(cars, nLastPreviewedCar);
	if (!record) return;

	AddCarToCareer(record->Handle);
}

void PresetCarSubmenu(const char** names, int numNames) {
	ChloeMenuLib::BeginMenu();
	auto cars = &UserProfile::spUserProfiles[0]->mCarStable;
	for (int i = 0; i < numNames; i++) {
		auto mdl = names[i];
		auto hash = Attrib::StringHash32(mdl);
		if (Attrib::FindCollection(0x27E73952, hash)) {
			auto existingRecord = FEPlayerCarDB::GetCarRecordByHandle(cars, hash);
			if (DrawMenuOption(std::format("{}{}", mdl, existingRecord && existingRecord->FilterBits != 0xF6910 ? "" : " (AI)"))) {
				ChloeMenuLib::BeginMenu();

				if (nLastPreviewedCar != hash) CreateAndPreviewAICar(hash);

				if (DrawMenuOption("Refresh Preview")) {
					CreateAndPreviewAICar(hash);
				}

				if (DrawMenuOption("Add to Career")) {
					AddCarToCareer(hash);
				}

				DrawMenuOption(std::format("Output: {}", lastState));

				ChloeMenuLib::EndMenu();
			}
		}
	}
	ChloeMenuLib::EndMenu();
}

void PresetCarEditor() {
	ChloeMenuLib::BeginMenu();

	if (TheGameFlowManager.CurrentGameFlowState != 3) {
		DrawMenuOption("Enter the main menu first!");
		ChloeMenuLib::EndMenu();
		return;
	}

	auto cars = &UserProfile::spUserProfiles[0]->mCarStable;

	if (DrawMenuOption("Preset Cars")) {
		ChloeMenuLib::BeginMenu();
		if (DrawMenuOption("ad_sales")) {
			const char* models[] = {
					"coke_gti",
					"energizer_viper",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("booster_opponents")) {
			const char* models[] = {
					"b_opp_1_drag",
					"b_opp_2_drag",
					"b_opp_3_drag",
					"b_opp_4_drag",
					"b_opp_5_drag",
					"b_opp_6_drag",
					"b_opp_7_drag",
					"b_opp_8_drag",
					"b_opp_9_drag",
					"b_opp_10_drag",
					"b_opp_11_drag",
					"b_opp_12_drag",
					"b_opp_13_drag",
					"b_opp_14_drag",
					"b_opp_15_drag",
					"b_opp_16_drag",
					"b_opp_17_drag",
					"b_opp_18_drag",
					"b_opp_19_drag",
					"b_opp_20_drag",
					"b_opp_21_drag",
					"b_opp_22_drag",
					"b_opp_1_drift",
					"b_opp_2_drift",
					"b_opp_3_drift",
					"b_opp_4_drift",
					"b_opp_5_drift",
					"b_opp_6_drift",
					"b_opp_7_drift",
					"b_opp_8_drift",
					"b_opp_9_drift",
					"b_opp_10_drift",
					"b_opp_11_drift",
					"b_opp_12_drift",
					"b_opp_13_drift",
					"b_opp_14_drift",
					"b_opp_15_drift",
					"b_opp_16_drift",
					"b_opp_17_drift",
					"b_opp_18_drift",
					"b_opp_19_drift",
					"b_opp_20_drift",
					"b_opp_21_drift",
					"b_opp_22_drift",
					"b_opp_1_grip",
					"b_opp_2_grip",
					"b_opp_3_grip",
					"b_opp_4_grip",
					"b_opp_5_grip",
					"b_opp_6_grip",
					"b_opp_7_grip",
					"b_opp_8_grip",
					"b_opp_9_grip",
					"b_opp_10_grip",
					"b_opp_11_grip",
					"b_opp_12_grip",
					"b_opp_13_grip",
					"b_opp_14_grip",
					"b_opp_15_grip",
					"b_opp_16_grip",
					"b_opp_17_grip",
					"b_opp_18_grip",
					"b_opp_19_grip",
					"b_opp_20_grip",
					"b_opp_21_grip",
					"b_opp_22_grip",
					"b_opp_1_sc",
					"b_opp_2_sc",
					"b_opp_3_sc",
					"b_opp_4_sc",
					"b_opp_5_sc",
					"b_opp_6_sc",
					"b_opp_7_sc",
					"b_opp_8_sc",
					"b_opp_9_sc",
					"b_opp_10_sc",
					"b_opp_11_sc",
					"b_opp_12_sc",
					"b_opp_13_sc",
					"b_opp_14_sc",
					"b_opp_15_sc",
					"b_opp_16_sc",
					"b_opp_17_sc",
					"b_opp_18_sc",
					"b_opp_19_sc",
					"b_opp_20_sc",
					"b_opp_21_sc",
					"b_opp_22_sc",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("commercial")) {
			const char* models[] = {
					"commercial_ae86",
					"commercial_camaro",
					"commercial_cuda",
					"commercial_e92",
					"commercial_evo",
					"commercial_fordgt",
					"commercial_gt2",
					"commercial_mustang",
					"commercial_mustang_old",
					"commercial_proto",
					"commercial_rx7",
					"commercial_s15",
					"commercial_s4",
					"commercial_wrx",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("debug_presets")) {
			const char* models[] = {
					"jacques_mobile",
					"jacques_mobile_drag",
					"jacques_mobile_grip",
					"jacques_mobile_speed",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("entourage")) {
			const char* models[] = {
					"drag_entourage_1_drag",
					"drag_entourage_2_drag",
					"drag_entourage_3_drag",
					"showdown_entourage_1_drag",
					"showdown_entourage_2_drag",
					"showdown_entourage_3_drag",
					"showdown_entourage_4_drag",
					"drift_entourage_1_drift",
					"drift_entourage_2_drift",
					"drift_entourage_3_drift",
					"showdown_entourage_2_drift",
					"grip_entourage_1_grip",
					"grip_entourage_2_grip",
					"grip_entourage_3_grip",
					"showdown_entourage_1_grip",
					"showdown_entourage_2_grip",
					"showdown_entourage_3_grip",
					"showdown_entourage_4_grip",
					"sc_entourage_1_sc",
					"sc_entourage_2_sc",
					"sc_entourage_3_sc",
					"showdown_entourage_1_sc",
					"showdown_entourage_2_sc",
					"showdown_entourage_3_sc",
					"showdown_entourage_4_sc",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("king")) {
			const char* models[] = {
					"drag_king",
					"drift_king",
					"grip_king",
					"sc_king",
					"showdown_king_final_drag",
					"showdown_king_final_drift",
					"showdown_king_final_grip",
					"showdown_king_final_sc",
					"showdown_king_playable",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("opponent")) {
			const char* models[] = {
					"ch_t1_ptl_drag_civichb",
					"ch_t1_ptl_drag_mustgt",
					"ch_t1_tx_drag_gti",
					"ch_t1_willow_drag_chevelle",
					"ch_t3_infineon_drag_cayman",
					"ch_t3_nevada_drag_z06",
					"ch_t1_tx_drift_350z",
					"ch_t1_willow_drift_chevelle",
					"ch_t1_willow_drift_is350",
					"ch_t2_autop_drift_s15",
					"ch_t2_autop_drift_solstice",
					"ch_t2_ebisu_drift_supra",
					"ch_t2_mond_drift_g35",
					"ch_t2_mond_drift_gto",
					"ch_t3_autob_drift_viper",
					"ch_t3_infineon_drift_nsx",
					"ch_t3_nevada_drift_z06",
					"ch_t1_ptl_grip_civichb",
					"ch_t1_tx_grip_350z",
					"ch_t1_tx_grip_gti",
					"ch_t1_willow_grip_is350",
					"ch_t2_autop_grip_s15",
					"ch_t2_ebisu_grip_bmwm3",
					"ch_t2_mond_grip_gto",
					"ch_t3_autob_grip_skyline",
					"ch_t3_infineon_grip_cayman",
					"ch_t3_infineon_grip_nsx",
					"ch_t3_nevada_grip_rs4",
					"ch_t2_ebisu_sc_bmwm3",
					"ch_t2_ebisu_sc_supra",
					"ch_t3_autob_sc_skyline",
					"ch_t3_autob_sc_viper",
					"ch_t3_nevada_sc_rs4",
					"ch_t3_nevada_sc_z06",
					"elite_opp_1_drag",
					"elite_opp_2_drag",
					"elite_opp_3_drag",
					"elite_opp_4_drag",
					"elite_opp_5_drag",
					"elite_opp_6_drag",
					"elite_opp_7_drag",
					"elite_opp_8_drag",
					"elite_opp_9_drag",
					"elite_opp_10_drag",
					"elite_opp_11_drag",
					"elite_opp_12_drag",
					"elite_opp_13_drag",
					"elite_opp_14_drag",
					"elite_opp_15_drag",
					"elite_opp_16_drag",
					"elite_opp_17_drag",
					"elite_opp_18_drag",
					"elite_opp_19_drag",
					"elite_opp_20_drag",
					"elite_opp_21_drag",
					"elite_opp_22_drag",
					"elite_opp_23_drag",
					"elite_opp_24_drag",
					"elite_opp_25_drag",
					"elite_opp_26_drag",
					"elite_opp_27_drag",
					"elite_opp_28_drag",
					"elite_opp_29_drag",
					"elite_opp_30_drag",
					"elite_opp_1_drift",
					"elite_opp_2_drift",
					"elite_opp_3_drift",
					"elite_opp_4_drift",
					"elite_opp_5_drift",
					"elite_opp_6_drift",
					"elite_opp_7_drift",
					"elite_opp_8_drift",
					"elite_opp_9_drift",
					"elite_opp_10_drift",
					"elite_opp_11_drift",
					"elite_opp_12_drift",
					"elite_opp_13_drift",
					"elite_opp_14_drift",
					"elite_opp_15_drift",
					"elite_opp_16_drift",
					"elite_opp_17_drift",
					"elite_opp_18_drift",
					"elite_opp_19_drift",
					"elite_opp_20_drift",
					"elite_opp_21_drift",
					"elite_opp_22_drift",
					"elite_opp_23_drift",
					"elite_opp_24_drift",
					"elite_opp_25_drift",
					"elite_opp_26_drift",
					"elite_opp_27_drift",
					"elite_opp_28_drift",
					"elite_opp_29_drift",
					"elite_opp_30_drift",
					"elite_opp_1_grip",
					"elite_opp_2_grip",
					"elite_opp_3_grip",
					"elite_opp_4_grip",
					"elite_opp_5_grip",
					"elite_opp_6_grip",
					"elite_opp_7_grip",
					"elite_opp_8_grip",
					"elite_opp_9_grip",
					"elite_opp_10_grip",
					"elite_opp_11_grip",
					"elite_opp_12_grip",
					"elite_opp_13_grip",
					"elite_opp_14_grip",
					"elite_opp_15_grip",
					"elite_opp_16_grip",
					"elite_opp_17_grip",
					"elite_opp_18_grip",
					"elite_opp_19_grip",
					"elite_opp_20_grip",
					"elite_opp_21_grip",
					"elite_opp_22_grip",
					"elite_opp_23_grip",
					"elite_opp_24_grip",
					"elite_opp_25_grip",
					"elite_opp_26_grip",
					"elite_opp_27_grip",
					"elite_opp_28_grip",
					"elite_opp_29_grip",
					"elite_opp_30_grip",
					"elite_opp_1_sc",
					"elite_opp_2_sc",
					"elite_opp_3_sc",
					"elite_opp_4_sc",
					"elite_opp_5_sc",
					"elite_opp_6_sc",
					"elite_opp_7_sc",
					"elite_opp_8_sc",
					"elite_opp_9_sc",
					"elite_opp_10_sc",
					"elite_opp_11_sc",
					"elite_opp_12_sc",
					"elite_opp_13_sc",
					"elite_opp_14_sc",
					"elite_opp_15_sc",
					"elite_opp_16_sc",
					"elite_opp_17_sc",
					"elite_opp_18_sc",
					"elite_opp_19_sc",
					"elite_opp_20_sc",
					"elite_opp_21_sc",
					"elite_opp_22_sc",
					"elite_opp_23_sc",
					"elite_opp_24_sc",
					"elite_opp_25_sc",
					"elite_opp_26_sc",
					"elite_opp_27_sc",
					"elite_opp_28_sc",
					"elite_opp_29_sc",
					"elite_opp_30_sc",
					"dday_opp_1_grip",
					"dday_opp_2_grip",
					"dday_opp_3_grip",
					"dday_opp_4_grip",
					"dday_opp_5_grip",
					"dday_opp_6_grip",
					"dday_opp_7_grip",
					"hd_opp_1_drag",
					"hd_opp_2_drag",
					"hd_opp_3_drag",
					"hd_opp_4_drag",
					"hd_opp_5_drag",
					"hd_opp_6_drag",
					"hd_opp_7_drag",
					"hd_opp_8_drag",
					"hd_opp_9_drag",
					"hd_opp_10_drag",
					"hd_opp_11_drag",
					"hd_opp_12_drag",
					"hd_opp_13_drag",
					"hd_opp_14_drag",
					"hd_opp_15_drag",
					"hd_opp_1_drift",
					"hd_opp_2_drift",
					"hd_opp_3_drift",
					"hd_opp_4_drift",
					"hd_opp_5_drift",
					"hd_opp_6_drift",
					"hd_opp_7_drift",
					"hd_opp_8_drift",
					"hd_opp_9_drift",
					"hd_opp_10_drift",
					"hd_opp_11_drift",
					"hd_opp_12_drift",
					"hd_opp_13_drift",
					"hd_opp_14_drift",
					"hd_opp_15_drift",
					"hd_opp_1_grip",
					"hd_opp_2_grip",
					"hd_opp_3_grip",
					"hd_opp_4_grip",
					"hd_opp_5_grip",
					"hd_opp_6_grip",
					"hd_opp_7_grip",
					"hd_opp_8_grip",
					"hd_opp_9_grip",
					"hd_opp_10_grip",
					"hd_opp_11_grip",
					"hd_opp_12_grip",
					"hd_opp_13_grip",
					"hd_opp_14_grip",
					"hd_opp_15_grip",
					"hd_opp_1_sc",
					"hd_opp_2_sc",
					"hd_opp_3_sc",
					"hd_opp_4_sc",
					"hd_opp_5_sc",
					"hd_opp_6_sc",
					"hd_opp_7_sc",
					"hd_opp_8_sc",
					"hd_opp_9_sc",
					"hd_opp_10_sc",
					"hd_opp_11_sc",
					"hd_opp_12_sc",
					"hd_opp_13_sc",
					"hd_opp_14_sc",
					"hd_opp_15_sc",
					"opp_1_drag",
					"opp_2_drag",
					"opp_3_drag",
					"opp_4_drag",
					"opp_5_drag",
					"opp_6_drag",
					"opp_7_drag",
					"opp_8_drag",
					"opp_9_drag",
					"opp_10_drag",
					"opp_11_drag",
					"opp_12_drag",
					"opp_13_drag",
					"opp_14_drag",
					"opp_15_drag",
					"opp_16_drag",
					"opp_17_drag",
					"opp_18_drag",
					"opp_19_drag",
					"opp_20_drag",
					"opp_21_drag",
					"opp_22_drag",
					"opp_23_drag",
					"opp_24_drag",
					"opp_25_drag",
					"opp_26_drag",
					"opp_27_drag",
					"opp_28_drag",
					"opp_29_drag",
					"opp_30_drag",
					"opp_31_drag",
					"opp_32_drag",
					"opp_33_drag",
					"opp_34_drag",
					"opp_35_drag",
					"opp_36_drag",
					"opp_37_drag",
					"opp_38_drag",
					"opp_39_drag",
					"opp_40_drag",
					"opp_41_drag",
					"opp_42_drag",
					"opp_43_drag",
					"opp_44_drag",
					"opp_45_drag",
					"opp_46_drag",
					"opp_47_drag",
					"opp_48_drag",
					"opp_49_drag",
					"opp_50_drag",
					"opp_51_drag",
					"opp_52_drag",
					"opp_53_drag",
					"opp_54_drag",
					"opp_1_drift",
					"opp_2_drift",
					"opp_3_drift",
					"opp_4_drift",
					"opp_5_drift",
					"opp_6_drift",
					"opp_7_drift",
					"opp_8_drift",
					"opp_9_drift",
					"opp_10_drift",
					"opp_11_drift",
					"opp_12_drift",
					"opp_13_drift",
					"opp_14_drift",
					"opp_15_drift",
					"opp_16_drift",
					"opp_17_drift",
					"opp_18_drift",
					"opp_19_drift",
					"opp_20_drift",
					"opp_21_drift",
					"opp_22_drift",
					"opp_23_drift",
					"opp_24_drift",
					"opp_25_drift",
					"opp_26_drift",
					"opp_27_drift",
					"opp_28_drift",
					"opp_29_drift",
					"opp_30_drift",
					"opp_31_drift",
					"opp_32_drift",
					"opp_33_drift",
					"opp_34_drift",
					"opp_35_drift",
					"opp_36_drift",
					"opp_37_drift",
					"opp_38_drift",
					"opp_39_drift",
					"opp_40_drift",
					"opp_41_drift",
					"opp_42_drift",
					"opp_43_drift",
					"opp_44_drift",
					"opp_45_drift",
					"opp_46_drift",
					"opp_47_drift",
					"opp_48_drift",
					"opp_49_drift",
					"opp_50_drift",
					"opp_51_drift",
					"opp_52_drift",
					"opp_53_drift",
					"opp_54_drift",
					"opp_1_grip",
					"opp_2_grip",
					"opp_3_grip",
					"opp_4_grip",
					"opp_5_grip",
					"opp_6_grip",
					"opp_7_grip",
					"opp_8_grip",
					"opp_9_grip",
					"opp_10_grip",
					"opp_11_grip",
					"opp_12_grip",
					"opp_13_grip",
					"opp_14_grip",
					"opp_15_grip",
					"opp_16_grip",
					"opp_17_grip",
					"opp_18_grip",
					"opp_19_grip",
					"opp_20_grip",
					"opp_21_grip",
					"opp_22_grip",
					"opp_23_grip",
					"opp_24_grip",
					"opp_25_grip",
					"opp_26_grip",
					"opp_27_grip",
					"opp_28_grip",
					"opp_29_grip",
					"opp_30_grip",
					"opp_31_grip",
					"opp_32_grip",
					"opp_33_grip",
					"opp_34_grip",
					"opp_35_grip",
					"opp_36_grip",
					"opp_37_grip",
					"opp_38_grip",
					"opp_39_grip",
					"opp_40_grip",
					"opp_41_grip",
					"opp_42_grip",
					"opp_43_grip",
					"opp_44_grip",
					"opp_45_grip",
					"opp_46_grip",
					"opp_47_grip",
					"opp_48_grip",
					"opp_49_grip",
					"opp_50_grip",
					"opp_51_grip",
					"opp_52_grip",
					"opp_53_grip",
					"opp_54_grip",
					"opp_1_sc",
					"opp_2_sc",
					"opp_3_sc",
					"opp_4_sc",
					"opp_5_sc",
					"opp_6_sc",
					"opp_7_sc",
					"opp_8_sc",
					"opp_9_sc",
					"opp_10_sc",
					"opp_11_sc",
					"opp_12_sc",
					"opp_13_sc",
					"opp_14_sc",
					"opp_15_sc",
					"opp_16_sc",
					"opp_17_sc",
					"opp_18_sc",
					"opp_19_sc",
					"opp_20_sc",
					"opp_21_sc",
					"opp_22_sc",
					"opp_23_sc",
					"opp_24_sc",
					"opp_25_sc",
					"opp_26_sc",
					"opp_27_sc",
					"opp_28_sc",
					"opp_29_sc",
					"opp_30_sc",
					"opp_31_sc",
					"opp_32_sc",
					"opp_33_sc",
					"opp_34_sc",
					"opp_35_sc",
					"opp_36_sc",
					"opp_37_sc",
					"opp_38_sc",
					"opp_39_sc",
					"opp_40_sc",
					"opp_41_sc",
					"opp_42_sc",
					"opp_43_sc",
					"opp_44_sc",
					"opp_45_sc",
					"opp_46_sc",
					"opp_47_sc",
					"opp_48_sc",
					"opp_49_sc",
					"opp_50_sc",
					"opp_51_sc",
					"opp_52_sc",
					"opp_53_sc",
					"opp_54_sc",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("player_presets")) {
			const char* models[] = {
					"player_ch_t1_ptl_grip_civichb",
					"player_ch_t1_ptl_grip_mustgt",
					"player_d_day",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("screenshot")) {
			const char* models[] = {
					"screenshot_240sx_grip_1",
					"screenshot_911t_grip_1",
					"screenshot_ae86_grip_1",
					"screenshot_corvette_grip_1",
					"screenshot_cuda_grip_1",
					"screenshot_elise_grip_1",
					"screenshot_g35_grip_1",
					"screenshot_gt500_grip_1",
					"screenshot_is350_grip_1",
					"screenshot_kobel_s15_grip_1",
					"screenshot_mustang_grip_1",
					"screenshot_rsx_grip_1",
					"screenshot_rx7_grip_1",
					"screenshot_rx8_grip_1",
					"screenshot_s15_grip_1",
					"screenshot_skyline_grip_1",
					"screenshot_supra_grip_1",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("temp_for_challenge")) {
			const char* models[] = {
					"ch_t3_inf_drag",
					"ch_t3_inf_drift",
					"ch_t3_inf_grip",
					"ch_t3_inf_grip2",
					"p_ch_t1_nvd_drag",
					"p_ch_t1_nvd_drag2",
					"p_ch_t1_nvd_grip",
					"p_ch_t1_tx_drag",
					"p_ch_t1_tx_drift",
					"p_ch_t1_tx_grip",
					"p_ch_t1_tx_grip2",
					"p_ch_t1_willow_drag",
					"p_ch_t1_willow_drag2",
					"p_ch_t1_willow_drift",
					"p_ch_t1_willow_grip",
					"p_ch_t2_autop_drift",
					"p_ch_t2_autop_drift2",
					"p_ch_t2_autop_grip",
					"p_ch_t2_ce_autop_drift",
					"p_ch_t2_ce_autop_grip",
					"p_ch_t2_ce_autop_grip2",
					"p_ch_t2_eb_drift",
					"p_ch_t2_eb_grip",
					"p_ch_t2_eb_sc",
					"p_ch_t2_eb_sc2",
					"p_ch_t3_autob_drift",
					"p_ch_t3_autob_grip",
					"p_ch_t3_autob_sc",
					"p_ch_t3_autob_sc2",
					"p_ch_t3_nev_drag",
					"p_ch_t3_nev_drift",
					"p_ch_t3_nev_grip",
					"p_ch_t3_nev_sc",
					"p_ch_t3_nev_sc2",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		if (DrawMenuOption("webtool")) {
			const char* models[] = {
					"webtool_focus_1",
					"webtool_focus_2",
					"webtool_focus_3",
					"webtool_rsx_1",
					"webtool_rsx_2",
					"webtool_rsx_3",
					"webtool_sti_1",
					"webtool_sti_2",
					"webtool_sti_3",
					"webtool_ttn_1",
					"webtool_ttn_2",
					"webtool_ttn_3",
			};
			PresetCarSubmenu(models, sizeof(models)/sizeof(models[0]));
		}
		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Add Preset Car Manually")) {
		ChloeMenuLib::BeginMenu();
		static char tmp[1024] = "";
		ChloeMenuLib::AddTextInputToString(tmp, 1024, false);
		DrawMenuOption(std::format("Name: {}", tmp));
		auto hash = Attrib::StringHash32(tmp);
		if (tmp[0] && Attrib::FindCollection(0x27E73952, hash)) {
			if (DrawMenuOption("Add Car to Career")) {
				CreateAndPreviewAICar(hash);
				AddCarToCareer(hash);
			}
		}
		if (!lastState.empty()) {
			DrawMenuOption(std::format("Output: {}", lastState));
		}
		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Make All Career Cars Tunable")) {
		for (auto& car : cars->CarTable) {
			auto customization = FEPlayerCarDB::GetCustomizationRecordByHandle(cars, car.Customization);
			auto career = FEPlayerCarDB::GetCareerRecordByHandle(cars, car.CareerHandle);
			if (customization != nullptr && career != nullptr) {
				customization->mBlueprintIsLocked[0] = customization->mBlueprintIsLocked[1] = customization->mBlueprintIsLocked[2] = false;
			}
		}
	}

	if (DrawMenuOption("Car Records (Debug)")) {
		ChloeMenuLib::BeginMenu();

		for (int i = 0; i < 410; i++) {
			auto car = &cars->CarTable[i];
			auto name = FECarRecord::GetDebugName(car);
			if (!name) continue;
			if (DrawMenuOption(std::format("{} - {}", i, name ? name : "(null)"))) {
				ChloeMenuLib::BeginMenu();

				DrawMenuOption(std::format("Handle - {:X}", car->Handle));
				DrawMenuOption(std::format("FEKey - {:X}", car->FEKey));
				DrawMenuOption(std::format("VehicleKey - {:X}", car->VehicleKey));
				DrawMenuOption(std::format("PresetKey - {:X}", car->PresetKey));
				DrawMenuOption(std::format("FilterBits - {:X}", car->FilterBits));
				DrawMenuOption(std::format("Customization - {}", car->Customization));
				DrawMenuOption(std::format("CareerHandle - {}", car->CareerHandle));
				if (DrawMenuOption(std::format("IsPresetSkin - {}", car->IsPresetSkin))) {
					car->IsPresetSkin = !car->IsPresetSkin;
				}

				if (car->Handle != 0xFFFFFFFF && DrawMenuOption("Preview")) {
					nFECarSetter = nLastPreviewedCar = car->Handle;
				}

				if (car->Handle != 0xFFFFFFFF && DrawMenuOption("Add Car to Career")) {
					AddCarToCareer(car->Handle);
				}

				ChloeMenuLib::EndMenu();
			}
		}

		ChloeMenuLib::EndMenu();
	}

	ChloeMenuLib::EndMenu();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x16AA080) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using v1.1 (.exe size of 3765248 bytes)", "nya?!~", MB_ICONERROR);
				return TRUE;
			}

			ChloeMenuLib::RegisterMenu("Preset Car Browser", &PresetCarEditor);
			NyaHooks::PlaceWorldServiceHook();
			NyaHooks::aWorldServiceFuncs.push_back(OnWorldService);
		} break;
		default:
			break;
	}
	return TRUE;
}
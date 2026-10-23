/**
 * Apex Structural Finite Element Node Stress Solver.
 * Author: Structural Computational Analyst
 * Description: Computes multi-axis structural vector loads and element safety margins.
 */

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

struct StructuralNode {
    int node_id;
    double coord_x;
    double coord_y;
};

struct MechanicalElement {
    int element_id;
    int start_node;
    int end_node;
    double cross_sectional_area; // Meters squared
    double applied_axial_force;  // Newtons
};

struct StructuralAnalysisResult {
    int element_id;
    double calculated_stress;    // Pascals
    double safety_margin;
    std::string safety_status;
};

class TrussSolverEngine {
private:
    std::vector<StructuralNode> structural_nodes;
    std::vector<MechanicalElement> structural_elements;
    double material_yield_strength; // Pascals (Pa)

public:
    TrussSolverEngine(double yield_strength) : material_yield_strength(yield_strength) {}

    void add_node(int id, double x, double y) {
        structural_nodes.push_back({id, x, y});
    }

    void add_element(int id, int start, int end, double area, double force) {
        structural_elements.push_back({id, start, end, area, force});
    }

    std::vector<StructuralAnalysisResult> compute_stress_profiles() {
        std::vector<StructuralAnalysisResult> results;

        for (const auto& element : structural_elements) {
            // Calculate stress: Force / Area
            double stress = element.applied_axial_force / element.cross_sectional_area;
            double absolute_stress = std::abs(stress);

            // Safety Margin = (Yield Strength / Actual Stress) - 1
            double margin = 0.0;
            std::string status;

            if (absolute_stress > 0.0) {
                margin = (material_yield_strength / absolute_stress) - 1.0;
            } else {
                margin = 999.0; // Infinite safety margin reference
            }

            if (margin < 0.0) {
                status = "STRUCTURAL_FAILURE_RISK";
            } else if (margin < 0.5) {
                status = "MARGINAL_WARNING";
            } else {
                status = "DESIGN_COMPLIANT";
            }

            results.push_back({element.element_id, stress, margin, status});
        }
        return results;
    }
};

int main() {
    // Instantiate engine for A36 Structural Steel (Yield Strength = 250 MPa / 2.5e8 Pa)
    TrussSolverEngine solver(250000000.0);

    // Node layout setup
    solver.add_node(1, 0.0, 0.0);
    solver.add_node(2, 4.0, 0.0);
    solver.add_node(3, 2.0, 3.0);

    // Element configuration (ID, StartNode, EndNode, Area m^2, Force N)
    // Element 1: High tension load
    solver.add_element(101, 1, 2, 0.0015, 120000000.0);
    // Element 2: Extreme overload scenario
    solver.add_element(102, 2, 3, 0.0012, 350000000.0);
    // Element 3: Stable load condition
    solver.add_element(103, 3, 1, 0.0015, -45000000.0);

    std::vector<StructuralAnalysisResult> analysis_reports = solver.compute_stress_profiles();

    std::cout << std::setw(10) << "Elem ID"
              << std::setw(18) << "Stress (MPa)"
              << std::setw(15) << "Margin"
              << std::setw(25) << "Status" << std::endl;
    std::cout << std::string(68, '-') << std::endl;

    for (const auto& report : analysis_reports) {
        std::cout << std::setw(10) << report.element_id
                  << std::setw(18) << (report.calculated_stress / 1000000.0)
                  << std::setw(15) << std::setprecision(3) << report.safety_margin
                  << std::setw(25) << report.safety_status << std::endl;
    }

    return 0;
}



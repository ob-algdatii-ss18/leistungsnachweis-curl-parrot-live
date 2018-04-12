#ifndef OUTPUTDATA_H
#define OUTPUTDATA_H

#include <memory>
#include <string>
#include <vector>
#include "Tensor.hpp"

/**
 * Node of the search tree that is created when exploring various car-ride distributions.
 * Each node can traverse its parent tree and create an output file.
 */
class OutputData : public std::enable_shared_from_this<OutputData> {
public:

	/**
	 * Creates a new node associated with a parent tree and containing a value.
	 *
	 * \param [in] parent parent node
	 * \param [in] value Tensor value of this node
	 */
	OutputData(std::shared_ptr<OutputData> parent, std::unique_ptr<Tensor> value) 
		:parent(parent), value(std::move(value)) {};

	/**
	 * Writes the value of this node and of all its parents to a specified file.
	 *
	 * \param [in] path specified file
	 * \param [in] fleetSize number of lines the file will contain
	 * \throw std::fstream::failure if file couldn't be opened/created
	 */
	void writeToFile(const std::string path, const unsigned fleetSize) const;

private:

	/** Parent node. */
	std::shared_ptr<OutputData> parent;
	/** Value of this node. */
	std::unique_ptr<Tensor> value;

	/**
	 * Creates a 2D vector of values while traversing parent nodes.
	 *
	 * \param [in] start first node for traversal
	 * \param [in] fleetSize number of vectors in 2D-vector
	 * \return std::vector<std::vector<int>>
	 */
	std::vector<std::vector<int>> createDataStructure(std::shared_ptr<const OutputData> start, const unsigned fleetSize) const;

};

#endif // OUTPUTDATA_H

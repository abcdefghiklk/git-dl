#define SEPARATOR "~~~~~~~~~~~~"
	fn += buf;
			    text_old += line + "\n";
			    text_new += line + "\n";
		    text_old += line + "\n";
		    text_new += line + "\n";
std::string commit(fast::Log_Commit * current_commit, std::string diff) {
	if (diff.length() > 1000000) {
		cout << "Ignoring the diff block because it is too big: "<< diff.length() << endl;
		return "";
	}
	std::string file_a;
	std::string file_b;
	std::string diff_line;
	if (current_commit != NULL) {
		// cout << "." << flush;
		fast::Log_Commit_Diff_Hunk * hunk = NULL;
		fast::Log_Commit_Diff * diff_record = NULL;
		size_t linePos = std::string::npos;
		std::string hunk_text = "";
		int lineno = 0;
		do {
		    linePos = diff.find("\n");
		    if (linePos != std::string::npos) {
			diff_line = diff.substr(0, linePos);
			diff = diff.substr(linePos + 1);
			bool is_special = false;
			size_t prefixPos = diff_line.find(DIFF_PREFIX);
			if (prefixPos != std::string::npos) {
			    std::string diff_prefix= diff_line.substr(0, prefixPos);
			    if (diff_prefix == "") {
				diff_line = diff_line.substr(prefixPos + strlen(DIFF_PREFIX));
				size_t fieldPos = diff_line.find(" b/");
				file_a = diff_line.substr(2, fieldPos - 2);
				file_b = diff_line.substr(fieldPos+3);
				diff_record = current_commit->add_diff();
				diff_record->set_a(file_a);
				diff_record->set_b(file_b);
				bool is_code = false;
				size_t extPos = file_a.rfind(".");
				if (extPos != std::string::npos) {
					std::string ext = file_a.substr(extPos+1);
					if (IS_CODE(ext)) {
						is_code = true;
						diff_record->set_is_code(ext);
					}
				} 
				if (!is_code) {
					extPos = file_b.rfind(".");
					if (extPos != std::string::npos) {
						std::string ext = file_b.substr(extPos+1);
						if (IS_CODE(ext)) {
							is_code = true;
							diff_record->set_is_code(ext);
						}
					}
				}
				if (! is_code) 
					diff_record->set_is_code("");
				is_special = true;
				hunk = NULL;
				hunk_text = "";
			    }
			}
			prefixPos = diff_line.find(INDEX_PREFIX);
			if (prefixPos != std::string::npos) {
			    std::string index_prefix= diff_line.substr(0, prefixPos);
			    if (index_prefix == "") {
				    // index aa28709..05acaef 100644
				    std::string index_line = diff_line.substr(prefixPos + strlen(INDEX_PREFIX));
				    size_t from_Pos = index_line.find("..");
				    size_t mode_Pos = index_line.find(" ");
				    if (from_Pos != std::string::npos) {
					std::string from_id = index_line.substr(0, from_Pos);
					std::string to_id = "";
					std::string mode = "";
					if (mode_Pos == std::string::npos) {
					    to_id = index_line.substr(from_Pos + 2);
					} else {
					    to_id = index_line.substr(from_Pos + 2).substr(0, mode_Pos - from_Pos - 2);
					    mode = index_line.substr(mode_Pos + 1);
					}
					if (from_id == "0000000")
						diff_record->set_is_new(true);
					else
						diff_record->set_is_new(false);
					diff_record->set_index_from(from_id);
					diff_record->set_index_to(to_id);
					diff_record->set_mode(mode);
				    }
				is_special = true;
			    }
			}
			prefixPos = diff_line.find(HUNK_PREFIX);
			if (prefixPos != std::string::npos) {
			    std::string hunk_prefix= diff_line.substr(0, prefixPos);
			    if (hunk_prefix == "") {
				    if (hunk != NULL && hunk_text != "") {
					if (diff_record->is_code() != "") {
					    process_hunk_xml(hunk, hunk_text, diff_record->is_code()); 
					} else {
					    process_hunk_text(hunk, hunk_text);
					}
					hunk = NULL;
					hunk_text = "";
				    }
				    // @@ -4,10 +4,13 @@ BLABLA
				    std::string hunk_line = diff_line.substr(prefixPos + strlen(HUNK_PREFIX));
				    size_t from_Pos = hunk_line.find("-");
				    size_t to_Pos = hunk_line.find("+");
				    size_t context_Pos = hunk_line.find(HUNK_PREFIX);
				    hunk = diff_record->add_hunk();
				    if (from_Pos != std::string::npos) {
					std::string from_id = hunk_line.substr(from_Pos+1, to_Pos - 1);
					size_t col_Pos = from_id.find(",");
					std::string from_lineno = from_id.substr(0, col_Pos);
					std::string from_column = from_id.substr(col_Pos+1);
					hunk->set_from_lineno(atoi(from_lineno.c_str()));
					hunk->set_from_column(atoi(from_column.c_str()));
				    }
				    if (to_Pos != std::string::npos) {
					std::string to_id = hunk_line.substr(to_Pos+1, context_Pos - 1);
					size_t col_Pos = to_id.find(",");
					std::string to_lineno = to_id.substr(0, col_Pos);
					std::string to_column = to_id.substr(col_Pos+1);
					hunk->set_to_lineno(atoi(to_lineno.c_str()));
					hunk->set_to_column(atoi(to_column.c_str()));
				    }
				    if (context_Pos != std::string::npos) {
					std::string context = hunk_line.substr(context_Pos + strlen(HUNK_PREFIX));
					hunk->set_context(context);
				    }
				    is_special = true;
			    }
			}
			prefixPos = diff_line.find(SKIP_PREFIX1);
			if (prefixPos != std::string::npos) {
			    std::string diff_prefix= diff_line.substr(0, prefixPos);
			    if (diff_prefix == "") {
				    is_special = true;
			    }
			}
			prefixPos = diff_line.find(SKIP_PREFIX2);
			if (prefixPos != std::string::npos) {
			    std::string diff_prefix= diff_line.substr(0, prefixPos);
			    if (diff_prefix == "") {
				    is_special = true;
			    }
			}
			prefixPos = diff_line.find(SKIP_PREFIX3);
			if (prefixPos != std::string::npos) {
			    std::string diff_prefix= diff_line.substr(0, prefixPos);
			    if (diff_prefix == "") {
				    is_special = true;
			    }
			}
			prefixPos = diff_line.find(SKIP_PREFIX4);
			if (prefixPos != std::string::npos) {
			    std::string diff_prefix= diff_line.substr(0, prefixPos);
			    if (diff_prefix == "") {
				    is_special = true;
			    }
			}
			if (! is_special)
				hunk_text += diff_line + "\n";
		    }
		    /*
		    lineno ++;
		    if (lineno > 100) {
			    cout << "." << flush;
			    lineno = 0;
		    }
		    */
		} while (linePos != std::string::npos);
	}
	return "";
}

	bool parallel = false;
	if (strcmp(argv[1], "-p") == 0) {
		parallel = true;
		argc-- ;
		argv++;
	}
	fn += buf;
	FILE *input0 = fopen(log_filename.c_str(), "w");
		if (strcmp(line.c_str(), SEPARATOR)==0) {
		fprintf(input0, "%s\n", line.c_str());
	fclose(input0);
	if (parallel) {
		int job;
		FILE **outputs = (FILE**) malloc(jobs * sizeof(FILE*));
		for (job=0; job< jobs; job++) {
			if (jobs != 1)
				sprintf(filename, "%s-%d.log", argv[1], job);
			else
				sprintf(filename, "%s.log", argv[1]);
			outputs[job] = fopen((char*) filename, "w");
		}
		job = 0;
		while(!input.eof()){
			std::getline(input, line);
			if (line == SEPARATOR) {
				if (no == (number+jobs-1)/jobs) {
					cout << "saved " << no << " records into " << argv[1] << "-" << job << ".log" << " ..." << endl;
					fclose(outputs[job]);
					job++;
					no = 0;
				}
				no++;
			}
			fprintf(outputs[job], "%s\n", line.c_str());
		}
		if (no != 0) {
			cout << "saved " << no << " records into " << argv[1] << "-" << job << ".log" << " ..." << endl;
			fclose(outputs[job]);
		}
		return 0;
	}
	int job = 0;
	// int lineno = 0;
		if (strcmp(line.c_str(), SEPARATOR)==0) {
			// std::cout << "." << std::flush;
				if (jobs != 1)
					sprintf(filename, "%s-%d.pb", argv[1], job);
				else
					sprintf(filename, "%s.pb", argv[1]);
			diff = commit(current_commit, diff);
			diff += line;
			diff += "\n";
	if (jobs != 1)
		sprintf(filename, "%s-%d.pb", argv[1], job);
	else
		sprintf(filename, "%s.pb", argv[1]);
		diff = commit(current_commit, diff);
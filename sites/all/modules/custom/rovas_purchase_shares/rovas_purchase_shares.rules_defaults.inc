<?php

/**
 * @file
 * rovas_purchase_shares.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function rovas_purchase_shares_default_rules_configuration() {
  $items = array();
  $items['rules_check_or_add_time_shareholder_core'] = entity_import('rules_config', '{ "rules_check_or_add_time_shareholder_core" : {
      "LABEL" : "Check or add time shareholder core",
      "PLUGIN" : "rule",
      "OWNER" : "rules",
      "REQUIRES" : [ "rules", "views_bulk_operations", "rules_conditional" ],
      "USES VARIABLES" : {
        "project" : { "label" : "project", "type" : "node" },
        "user" : { "label" : "user", "type" : "user" },
        "result" : { "label" : "result", "type" : "integer", "parameter" : false }
      },
      "IF" : [
        { "entity_is_of_bundle" : {
            "entity" : [ "project" ],
            "type" : "node",
            "bundle" : { "value" : { "project" : "project" } }
          }
        }
      ],
      "DO" : [
        { "views_bulk_operations_action_load_list" : {
            "USING" : {
              "view" : "project_user_time_shareholder|default",
              "args" : "[project:nid]\\r\\n[user:uid]"
            },
            "PROVIDE" : { "entity_list" : { "entity_list" : "A list of entities" } }
          }
        },
        { "CONDITIONAL" : [
            {
              "IF" : { "list_count_is" : { "list" : [ "entity-list" ], "op" : "\\u003E", "value" : "0" } },
              "DO" : [
                { "data_set" : { "data" : [ "result" ], "value" : [ "entity-list:0:nid" ] } }
              ]
            },
            { "ELSE" : [
                { "entity_create" : {
                    "USING" : {
                      "type" : "node",
                      "param_type" : "timeshare_merit_allocation",
                      "param_title" : "temp",
                      "param_author" : [ "project:author" ]
                    },
                    "PROVIDE" : { "entity_created" : { "wma" : "Created wma" } }
                  }
                },
                { "entity_modify" : {
                    "entity" : [ "wma" ],
                    "properties" : { "value" : { "field_coworker" : "field_coworker", "field_project" : "field_project" } },
                    "param_field_coworker" : [ "user" ],
                    "param_field_project" : [ "project" ]
                  }
                },
                { "entity_save" : { "data" : [ "wma" ], "immediate" : "1" } },
                { "data_set" : { "data" : [ "result" ], "value" : [ "wma:nid" ] } }
              ]
            }
          ]
        }
      ],
      "PROVIDES VARIABLES" : [ "result" ]
    }
  }');
  $items['rules_create_time_share'] = entity_import('rules_config', '{ "rules_create_time_share" : {
      "LABEL" : "Create time share",
      "PLUGIN" : "rule",
      "OWNER" : "rules",
      "REQUIRES" : [ "rules" ],
      "USES VARIABLES" : {
        "project" : { "label" : "Parent project", "type" : "node" },
        "p_receipt" : { "label" : "Purchase receipt", "type" : "node" },
        "chrons" : { "label" : "Chrons paid", "type" : "decimal" },
        "hours" : { "label" : "Hours purchased", "type" : "decimal" },
        "note" : { "label" : "Note", "type" : "text", "allow null" : true },
        "nid" : { "label" : "Timeshare ID", "type" : "integer", "parameter" : false }
      },
      "IF" : [
        { "entity_is_of_bundle" : {
            "entity" : [ "p-receipt" ],
            "type" : "node",
            "bundle" : { "value" : { "purchase_receipt" : "purchase_receipt" } }
          }
        }
      ],
      "DO" : [
        { "entity_create" : {
            "USING" : {
              "type" : "node",
              "param_type" : "time_share",
              "param_title" : "tmp",
              "param_author" : [ "p-receipt:field-buyer" ]
            },
            "PROVIDE" : { "entity_created" : { "time_share" : "time share" } }
          }
        },
        { "entity_modify" : {
            "entity" : [ "time-share" ],
            "properties" : { "value" : {
                "field_project" : "field_project",
                "field_date" : "field_date",
                "field_chrons_paid" : "field_chrons_paid",
                "field_hours_purchased" : "field_hours_purchased",
                "field_purchase_proof" : "field_purchase_proof",
                "language" : "language",
                "created" : "created"
              }
            },
            "param_field_project" : [ "project" ],
            "param_field_chrons_paid" : [ "chrons" ],
            "param_field_hours_purchased" : [ "hours" ],
            "param_field_date" : [ "p-receipt:created" ],
            "param_field_purchase_proof" : [ "p-receipt" ],
            "param_language" : [ "p-receipt:language" ],
            "param_created" : [ "p-receipt:created" ]
          }
        },
        { "entity_save" : { "data" : [ "time-share" ], "immediate" : "1" } },
        { "data_set" : { "data" : [ "nid" ], "value" : [ "time-share:nid" ] } }
      ],
      "PROVIDES VARIABLES" : [ "nid" ]
    }
  }');
  return $items;
}

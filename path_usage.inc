<?php

function path_usage_find_paths($object, $object_id, $object_type, $object_subtype = NULL) {
  $tracker = drupal_static(__FUNCTION__, array());
  $subrequest = FALSE;
  // we use this to check for infinite recursion
  $object_uuid = hash('sha256', $object_type . $object_subtype . $object_id);
  if (!empty($tracker['in_progress'])) {
    $subrequest = TRUE;
    if (in_array($object_uuid, $tracker['uuids'])) {
      return array();
    }
  } else {
    $tracker['in_progress'] = TRUE;
  }
  $tracker['uuids'][] = $object_uuid;
  $object_type = _path_usage_clean_for_hook($object_type);
  $object_subtype = _path_usage_clean_for_hook($object_subtype);
  $pages = array();
  // fire hook_path_usages()
  _path_usage_global_hook($pages, $object, $object_type, $object_subtype);
  // fire hook_TYPE_path_usages() and hook_TYPE_SUBTYPE_path_usages().
  _path_usage_specific_hook($pages, $object, $object_type, $object_subtype);
  $pages = array_unique($pages);
  // fire hook_path_usages_alter().
  _path_usage_global_alter_hook($pages, $object, $object_type, $object_subtype);
  // fire hook_TYPE_path_usages_alter() and hook_TYPE_SUBTYPE_page_usages_alter().
  _path_usage_specific_alter_hook($pages, $object, $object_type, $object_subtype);
  if (!$subrequest) {
    unset($tracker['in_progress']);
  }
  return array_unique($pages);
}

function _path_usage_global_hook(&$pages, $object, $object_type, $object_subtype = NULL) {
  $modules = module_implements('path_usages');
  foreach ($modules as $module) {
    $hook = $module . '_path_usages';
    if (function_exists($hook)) {
      $results =  $hook($object, $object_type, $object_subtype);
      if (!empty($results)) {
        $pages += $results;
      }
    }
  }
}

function _path_usage_global_alter_hook(&$pages, $object, $object_type, $object_subtype = NULL) {
  $modules = module_implements('path_usages_alter');
  foreach ($modules as $module) {
    $hook = $module . '_path_usages_alter';
    if (function_exists($hook)) {
      $hook($pages, $object, $object_type, $object_subtype);
    }
  }
}

function _path_usage_specific_hook(&$pages, $object, $object_type, $object_subtype = NULL) {
  $modules = module_implements($object_type . '_path_usages');
  foreach ($modules as $module) {
    $hook = $module . '_' . $object_type . '_path_usages';
    if (function_exists($hook)) {
      $results = $hook($object, $object_subtype);
      if (!empty($results)) {
        $pages += $results;
      }
    }
  }
  $modules = module_implements($object_type . '_' . $object_subtype . '_path_usages');
  foreach ($modules as $module) {
    $hook = $module . '_' . $object_type . '_' . $object_subtype . '_path_usages';
    if (function_exists($hook)) {
      $results = $hook($object);
      if (!empty($results)) {
        $pages += $hook($object);
      }
    }
  }
}

function _path_usage_specific_alter_hook(&$pages, $object, $object_type, $object_subtype = NULL) {
  $modules = module_implements($object_type . '_path_usages_alter');
  foreach ($modules as $module) {
    $hook = $module . '_' . $object_type . '_path_usages_alter';
    if (function_exists($hook)) {
      $hook($pages, $object, $object_subtype);
    }
  }
  $modules = module_implements($object_type . '_' . $object_subtype . '_path_usages_alter');
  foreach ($modules as $module) {
    $hook = $module . '_' . $object_type . '_' . $object_subtype . '_path_usages_alter';
    if (function_exists($hook)) {
      $hook($pages, $object);
    }
  }
}

function _path_usage_clean_for_hook($string) {
  if (!is_string($string)) {
    return $string;
  }
  if (empty($string)) {
    return $string;
  }
  return
    str_replace('__', '_',
      str_replace(' ', '_',
        str_replace('-', '_',
          $string
        )
      )
    );
}